#
# (c) 2022 Copyright, Real-Time Innovations, Inc.  All rights reserved.
#
# RTI grants Licensee a license to use, modify, compile, and create derivative
# works of the Software solely for use with RTI products.  The Software is
# provided "as is", with no warranty of any type, including any warranty for
# fitness for any purpose. RTI is under no obligation to maintain or support
# the Software.  RTI shall not be liable for any incidental or consequential
# damages arising out of the use or inability to use the software.
#

from dataclasses import dataclass, field
from typing import Sequence

import pytest

import rti.idl as idl
import rti.idl_impl.sample_interpreter as sample_interpreter

from test_utils.fixtures import *

@idl.struct(member_annotations={
    'name': [idl.bound(11), idl.utf16]
})
class Person:
    name: str = ""
    age: Optional[idl.int16] = None


@idl.struct(member_annotations={
    'tasks': [idl.bound(10), idl.element_annotations([idl.bound(20)])]
})
class Worker(Person):
    tasks: Sequence[str] = field(default_factory=list)


@idl.struct
class Employee(Worker):
    company: str = ""


def create_sample():
    return Employee(name="John", age=30, tasks=['Cleaning', 'Coffee'], company="RTI")


@pytest.fixture
def sample():
    return create_sample()


def test_plugin33():
    ts = idl.get_type_support(Employee)
    assert ts.type is Employee

    dt = ts.dynamic_type
    assert dt.name == "Employee"
    assert len(dt.members()) == 1

    worker_ts = idl.get_type_support(Worker)
    assert dt.parent == worker_ts.dynamic_type

    person_ts = idl.get_type_support(Person)
    assert worker_ts.dynamic_type.parent == person_ts.dynamic_type
    assert len(person_ts.dynamic_type.members()) == 2

    # The annotations of the base type must be used to create the program
    program = ts._sample_programs.py_to_c_program
    assert type(program.instructions[0]) is sample_interpreter.CopyWStrToBytesInstruction
    assert program.instructions[0].bound == 11


def test_default_serialization():
    ts = idl.get_type_support(Employee)
    buffer = ts.serialize(Employee())
    deserialized_sample = ts.deserialize(buffer)
    assert Employee() == deserialized_sample


def test_serialization(sample):
    ts = idl.get_type_support(Employee)
    buffer = ts.serialize(sample)
    deserialized_sample = ts.deserialize(buffer)
    assert sample == deserialized_sample


def test_pubsub(shared_participant, sample):
    fixture = PubSubFixture(shared_participant, Employee)
    fixture.send_and_check([sample, Employee(), sample])


def test_base_serialization_fails_when_out_of_bounds():
    ts = idl.get_type_support(Employee)

    ts.serialize(Employee(name="a" * 11, tasks=["b" * 20] * 10))
    with pytest.raises(Exception) as ex:
        ts.serialize(Employee(name="a" * 12))
    assert "Error processing field 'name'" in str(ex.value)

    with pytest.raises(Exception) as ex:
        ts.serialize(Employee(tasks=["b" * 21]))
    assert "Error processing field 'tasks'" in str(ex.value)

    with pytest.raises(Exception) as ex:
        ts.serialize(Employee(tasks=["b"] * 11))
    assert "Error processing field 'tasks'" in str(ex.value)


def test_inconsistent_base_annotation():
    @idl.struct(type_annotations=[idl.final])
    class A:
        x: int = 0

    with pytest.raises(Exception):
        @idl.struct(type_annotations=[idl.mutable])
        class _B(A):
            x: int = 0

def test_empty_derived_type():
    @idl.struct
    class A:
        x: int = 0

    @idl.struct
    class B(A):
        pass

    ts = idl.get_type_support(B)
    dt = ts.dynamic_type

    assert dt.parent == idl.get_type_support(A).dynamic_type
    assert len(dt.members()) == 0
    assert B(4) == ts.deserialize(ts.serialize(B(4)))

def test_non_idl_base_type():
    @dataclass
    class Animal:
        name: str = ""

    @idl.struct
    class Dog(Animal):
        breed: str = ""

    ts = idl.get_type_support(Dog)
    dt = ts.dynamic_type

    # Since Animal is not an IDL type, the DynamicType for Dog has no parent
    # but contains the members of Animal
    assert not dt.has_parent
    assert len(dt.members()) == 2

    sample = Dog(name="Fido", breed="Labrador")
    assert sample == ts.deserialize(ts.serialize(sample))


def test_multiple_base_types():
    @dataclass
    class Animal:
        name: str = ""

    @idl.struct
    class Pet:
        owner: str = ""

    # Only the first base can be a DDS type, but the @idl.struct decorator
    # doesn't disallow additional base types
    @idl.struct
    class Dog(Pet, Animal):
        breed: str = ""

    ts = idl.get_type_support(Dog)
    dt = ts.dynamic_type

    # From the DDS point of view, Dog has one parent: Pet; the members of Animal
    # are in Dog's DynamicType
    assert dt.parent == idl.get_type_support(Pet).dynamic_type
    assert len(dt.members()) == 2
    assert dt["breed"] is not None
    assert dt["owner"] is not None

    sample = Dog(name="Fido", breed="Labrador", owner="John")
    assert sample == ts.deserialize(ts.serialize(sample))
