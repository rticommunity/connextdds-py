#
# (c) 2021 Copyright, Real-Time Innovations, Inc.  All rights reserved.
#
# RTI grants Licensee a license to use, modify, compile, and create derivative
# works of the Software solely for use with RTI products.  The Software is
# provided "as is", with no warranty of any type, including any warranty for
# fitness for any purpose. RTI is under no obligation to maintain or support
# the Software.  RTI shall not be liable for any incidental or consequential
# damages arising out of the use or inability to use the software.
#

from typing import Sequence, Optional
from dataclasses import field, dataclass
from array import array
import numpy as np

import pytest

import rti.connextdds as dds
import rti.types as idl
from rti.types.builtin import KeyedString


from common_types import Point
from test_utils.fixtures import *
from test_utils import log_capture


@idl.struct(
    member_annotations={
        'prices': [idl.bound(4)],
        'prices_array': [idl.bound(4)],
        'vertices_bounded': [idl.bound(2)]
    }
)
class SequenceTest:
    vertices: Sequence[Point] = field(default_factory=list)
    weights: Sequence[int] = field(default_factory=list)
    prices: Sequence[float] = field(default_factory=list)
    ready: Sequence[bool] = field(default_factory=list)

    # These are IDL sequences, but use array instead of list in py for better
    # performance. Our mapping supports both for primitive types.
    weights_array: Sequence[idl.int16] = field(
        default_factory=idl.array_factory(idl.int16))
    prices_array: Sequence[float] = field(
        default_factory=idl.array_factory(float))
    ready_array: Sequence[bool] = field(
        default_factory=idl.array_factory(bool))

    vertices_bounded: Sequence[Point] = field(default_factory=list)


@idl.struct(
    member_annotations={
        'prices': [idl.bound(4)],
        'prices_array': [idl.bound(4)],
        'vertices_bounded': [idl.bound(2)]
    }
)
class OptionalSequenceTest:
    vertices: Optional[Sequence[Point]] = None
    weights: Optional[Sequence[int]] = None
    prices: Optional[Sequence[float]] = None
    ready: Optional[Sequence[bool]] = None
    weights_array: Optional[Sequence[idl.int16]] = None
    prices_array: Optional[Sequence[float]] = None
    ready_array: Optional[Sequence[bool]] = None
    vertices_bounded: Optional[Sequence[Point]] = None


def create_sequence_sample():
    return SequenceTest(
        vertices=[Point(10, 20), Point(30, 40)],
        weights=[1, 2, 3],
        prices=[1.5, 2.5, 3.5, 4.5],
        ready=[True, False, True, True],
        weights_array=array('h', [111, 222]),
        prices_array=array('d', [11.5, 22.5, 33.5]),
        ready_array=array('b', [False, True]),
        vertices_bounded=[Point(100, 200)])


def create_optional_sequence_sample():
    return OptionalSequenceTest(
        vertices=[Point(10, 20), Point(30, 40)],
        weights=dds.Int64Seq([1, 2, 3]),
        prices=array('d', [1.5, 2.5, 3.5, 4.5]),
        ready=array('b', [True, False, True, True]),
        weights_array=array('h', [111, 222]),
        prices_array=dds.DoubleSeq([11.5, 22.5, 33.5]),
        ready_array=array('b', [False, True]),
        vertices_bounded=[Point(100, 200)])


@pytest.fixture
def sequence_sample():
    return create_sequence_sample()


@pytest.fixture
def optional_sequence_sample():
    return create_optional_sequence_sample()


def test_sequence_plugin():
    ts = idl.get_type_support(SequenceTest)
    assert ts.type is SequenceTest

    dt = ts.dynamic_type
    assert dt.name == "SequenceTest"
    assert len(dt.members()) == 8

    point_ts = idl.get_type_support(Point)
    assert dt["vertices"].type == dds.SequenceType(point_ts.dynamic_type)
    assert dt["weights"].type == dds.SequenceType(dds.Int64Type())
    assert dt["prices"].type == dds.SequenceType(dds.Float64Type(), 4)
    assert dt["ready"].type == dds.SequenceType(dds.BoolType())
    assert dt["weights_array"].type == dds.SequenceType(dds.Int16Type())
    assert dt["prices_array"].type == dds.SequenceType(dds.Float64Type(), 4)
    assert dt["ready_array"].type == dds.SequenceType(dds.BoolType())
    assert dt["vertices_bounded"].type == dds.SequenceType(point_ts.dynamic_type, 2)

    assert ts.is_unbounded


def test_native_sequence_initialization(sequence_sample):
    ts = idl.get_type_support(SequenceTest)
    c_sample = ts.c_type()

    # The initialize function sets the correct absolute maximum
    ts._plugin_dynamic_type.initialize_sample(c_sample)
    try:
        assert c_sample.vertices._absolute_maximum == idl.annotations.UNBOUNDED
        assert c_sample.prices._absolute_maximum == 4
        assert len(c_sample.vertices) == 0
        assert len(c_sample.prices) == 0
    finally:
        ts._plugin_dynamic_type.finalize_sample(c_sample)

def test_sequence_serialization(sequence_sample):
    ts = idl.get_type_support(SequenceTest)
    buffer = ts.serialize(sequence_sample)
    deserialized_sample = ts.deserialize(buffer)
    assert sequence_sample == deserialized_sample


def test_sequence_list_comprehension_serialization():
    ts = idl.get_type_support(SequenceTest)
    sequence_sample = SequenceTest()
    sequence_sample.vertices = [Point(x, 0) for x in range(3)]
    sequence_sample.weights = range(3)
    buffer = ts.serialize(sequence_sample)
    deserialized_sample = ts.deserialize(buffer)
    assert list(sequence_sample.vertices) == deserialized_sample.vertices
    assert list(sequence_sample.weights) == deserialized_sample.weights


@pytest.mark.parametrize("SequenceTestType", [SequenceTest, OptionalSequenceTest])
def test_empty_sequence_serialization(SequenceTestType):
    ts = idl.get_type_support(SequenceTestType)
    buffer = ts.serialize(SequenceTestType())
    deserialized_sample = ts.deserialize(buffer)
    assert SequenceTestType() == deserialized_sample


def test_sequence_pubsub(shared_participant, sequence_sample):
    fixture = PubSubFixture(shared_participant, SequenceTest, reader_policies=[
                            dds.ResourceLimits(1, 1, 1)])
    fixture.send_and_check(SequenceTest())
    fixture.send_and_check(sequence_sample)
    fixture.send_and_check(SequenceTest())
    sequence_sample.vertices_bounded += [Point(7, 8)]
    sequence_sample.vertices += [Point(7, 8)]
    fixture.send_and_check(sequence_sample)


def test_optional_sequence_serialization(optional_sequence_sample):
    ts = idl.get_type_support(OptionalSequenceTest)
    buffer = ts.serialize(optional_sequence_sample)
    deserialized_sample = ts.deserialize(buffer)
    assert optional_sequence_sample == deserialized_sample


def test_optional_sequence_pubsub(shared_participant, optional_sequence_sample):
    fixture = PubSubFixture(shared_participant, OptionalSequenceTest)
    fixture.send_and_check(optional_sequence_sample)
    fixture.send_and_check(OptionalSequenceTest())
    optional_sequence_sample.prices_array += idl.to_array(float, [7.23])
    optional_sequence_sample.vertices_bounded += [Point(7, 8)]
    fixture.send_and_check(optional_sequence_sample)

@pytest.mark.parametrize("SequenceTestType", [SequenceTest, OptionalSequenceTest])
def test_sequence_serialization_fails_when_out_of_bounds(SequenceTestType):
    ts = idl.get_type_support(SequenceTestType)

    sample = SequenceTestType(prices=[1.5, 2.5, 3.5, 4.5])
    ts.serialize(sample)
    sample.prices += [5.5]

    # fail_if_no_logs is set to False because due to impl. details the bounds
    # check for optionals is done in C, but for non-optionals it is done in
    # Python, and therefore in both cases there's an exception, but only in the
    # C layer an additional stdout log message is printed.
    with log_capture.expected_exception(idl.FieldSerializationError, fail_if_no_logs=False) as errinfo:
        ts.serialize(sample)
    assert "Error processing field 'prices'" in str(errinfo.exception_msg)

    sample = SequenceTestType(vertices_bounded=[Point(1, 2), Point(3, 4)])
    ts.serialize(sample)
    sample.vertices_bounded += [Point(5, 6)]
    with log_capture.expected_exception(idl.FieldSerializationError, fail_if_no_logs=False) as errinfo:
        ts.serialize(sample)
    assert "Error processing field 'vertices_bounded'" in str(
        errinfo.exception_msg)


@pytest.mark.parametrize("SequenceTestType", [SequenceTest, OptionalSequenceTest])
def test_sequence_serialization_fails_with_incorrect_element_type(SequenceTestType):
    ts = idl.get_type_support(SequenceTestType)

    sample = SequenceTestType(prices=[1.5, "hello"])
    with pytest.raises(idl.FieldSerializationError) as ex:
        ts.serialize(sample)
    assert "Error processing field 'prices'" in str(ex.value)

    sample = SequenceTestType(vertices=[Point(1, 2), 3])
    with pytest.raises(idl.FieldSerializationError) as ex:
        ts.serialize(sample)
    assert "Error processing field 'vertices'" in str(ex.value)


@pytest.mark.skip("Enable once a buffer-like field can be serialized as a list")
def test_sequence_serialization_with_int_element_out_of_range_is_safe():
    ts = idl.get_type_support(SequenceTest)

    # The current implementation doesn't enforce int sizes, but elements within
    # the correct range will be correctly serialized an deserialized
    sample = SequenceTest(weights_array=array([2**15 - 1, 2**15, 4]))
    deserialized_sample = ts.deserialize(ts.serialize(sample))

    assert deserialized_sample.weights_array[0] == sample.weights_array[0]
    assert deserialized_sample.weights_array[1] != sample.weights_array[1]
    assert deserialized_sample.weights_array[2] == sample.weights_array[2]


def test_sequence_iterator_serialization():
    ts = idl.get_type_support(SequenceTest)

    # The serialization routines support any sequence of elements, including
    # a lazy iterator such as range()
    sample = SequenceTest(weights=range(2, 5))
    buffer = ts.serialize(sample)
    deserialized_sample = ts.deserialize(buffer)
    assert deserialized_sample.weights == [2, 3, 4]


@pytest.mark.parametrize("SequenceTestType", [SequenceTest, OptionalSequenceTest])
def test_sequence_publication_fails_when_out_of_bounds(shared_participant, SequenceTestType):
    fixture = PubSubFixture(shared_participant, SequenceTestType)
    with log_capture.expected_exception(idl.FieldSerializationError, fail_if_no_logs=False) as errinfo:
        fixture.writer.write(SequenceTestType(prices=[1.1] * 5))
    assert "Error processing field 'prices'" in str(errinfo.exception_msg)


def test_sequence_of_sequence_not_supported():
    with pytest.raises(TypeError) as ex:
        @idl.struct
        class SeqOfSeq:
            s: Sequence[Sequence[Point]]

    assert "Sequences of sequences are not supported" in str(ex.value)


def test_serialize_primitive_list():

    # By default you can serialize a list of primitives even if by default
    # we deserialize primitive sequences as array.array's for better performance
    @idl.struct
    class List1:
        values: Sequence[int] = field(default_factory=idl.array_factory(int))

    ts = idl.get_type_support(List1)
    sample = List1([1, 2, 3])
    result = ts.deserialize(ts.serialize(sample))
    assert result.values == idl.to_array(int, [1, 2, 3])

    # You're allowed to disable primitive lists before a type is parsed,
    # which improves performance a bit (it saves an isinstance(member, list),
    # but now you can only assign arrays.
    idl.serialization_options.allow_primitive_lists = False

    try:
        @idl.struct
        class List2:
            values: Sequence[int] = field(
                default_factory=idl.array_factory(int))

        ts = idl.get_type_support(List2)
        sample = List2([1, 2, 3])
        with pytest.raises(idl.FieldSerializationError):
            ts.serialize(sample)

        sample = List2(idl.to_array(int, [1, 2, 3]))
        result = ts.deserialize(ts.serialize(sample))
        assert result == sample

    finally:
        idl.serialization_options.allow_primitive_lists = True


def test_set_list_array_factory():

    # Change the default array factory to use Python lists
    idl.type_utils.set_array_factory(idl.type_utils.list_factory)

    try:
        # array_factory now creates lists arrays
        @idl.struct
        class List1:
            int_values: Sequence[int] = field(
                default_factory=idl.array_factory(int))
            float32_values: Sequence[idl.float32] = field(
                default_factory=idl.array_factory(idl.float32))

        ts = idl.get_type_support(List1)
        sample = List1(
            int_values=[1, 2, 3],
            float32_values=np.array([1.0, 2.0, 3.0], dtype=np.float32))
        result = ts.deserialize(ts.serialize(sample))
        assert isinstance(result.int_values, list)
        assert isinstance(result.float32_values, list)
        assert result.int_values == [1, 2, 3]
        assert result.float32_values == [1.0, 2.0, 3.0]
    finally:
        idl.type_utils.reset_default_array_factory()

    # array_factory creates rti sequences again
    @idl.struct
    class List2:
        int_values: Sequence[int] = field(
            default_factory=idl.array_factory(int))
        float32_values: Sequence[idl.float32] = field(
            default_factory=idl.array_factory(idl.float32))

    ts = idl.get_type_support(List2)
    sample = List2(
        # The serialization supports lists...
        int_values=idl.to_array(int, [1, 2, 3]),
        # ... or arrays
        float32_values=[1.0, 2.0, 3.0])
    result = ts.deserialize(ts.serialize(sample))
    assert isinstance(result.int_values, dds.Int64Seq)
    assert isinstance(result.float32_values, dds.Float32Seq)
    assert result.int_values == sample.int_values
    assert result.float32_values == idl.to_array(idl.float32, [1.0, 2.0, 3.0])

IDL_TO_NUMPY_PRIMITIVE_TYPES = {
    idl.int8: np.int8,
    idl.int16: np.int16,
    idl.int32: np.int32,
    idl.int64: np.int64,
    idl.uint16: np.uint16,
    idl.uint32: np.uint32,
    idl.uint64: np.uint64,
    idl.float32: np.float32,
    idl.float64: np.float64,
    bool: np.int8,
}


@dataclass
class NumpyArrayFactory:

    element_type: type
    size: int = 0

    def __post_init__(self):
        # Translate from IDL primitive type to numpy primitive type
        self.element_type_numpy = IDL_TO_NUMPY_PRIMITIVE_TYPES.get(
            self.element_type)

        if self.element_type_numpy is None:
            raise TypeError(
                f"'{self.element_type}' is not a valid primitive element type for an array")

    def __call__(self, size=None):
        if self.size == 0:
            if size is not None:
                # A sequence of a specific size is requested
                return np.zeros(size, dtype=self.element_type_numpy)
            else:
                # An empty sequence is requested
                return np.zeros(0, dtype=self.element_type_numpy)
        else:
            # A fixed-size array is requested (i.e. a int[10] IDL array)
            return np.zeros(self.size, dtype=self.element_type_numpy)


def test_set_numpy_array_factory():

    # Change the default array factory to use numpy
    idl.type_utils.set_array_factory(NumpyArrayFactory)

    try:
        # array_factory now creates numpy arrays
        @idl.struct
        class List1:
            int_values: Sequence[int] = field(
                default_factory=idl.array_factory(int))
            float32_values: Sequence[idl.float32] = field(
                default_factory=idl.array_factory(idl.float32))

        ts = idl.get_type_support(List1)
        sample = List1(
            int_values=[1, 2, 3],
            float32_values=np.array([1.0, 2.0, 3.0], dtype=np.float32))
        result = ts.deserialize(ts.serialize(sample))
        assert isinstance(result.int_values, np.ndarray)
        assert isinstance(result.float32_values, np.ndarray)
        assert (result.int_values == [1, 2, 3]).all()
        assert (result.float32_values == [1.0, 2.0, 3.0]).all()
    finally:
        idl.type_utils.reset_default_array_factory()


def test_unbounded_writer_qos(shared_participant: dds.DomainParticipant):
    property_name = 'dds.data_writer.history.memory_manager.fast_pool.pool_buffer_max_size'

    # For unbounded types, the writer is automatically created with unbounded
    # memory management support
    assert idl.get_type_support(SequenceTest).is_unbounded

    topic = dds.Topic(shared_participant, "test_unbounded", SequenceTest)
    default_writer = dds.DataWriter(shared_participant, topic)
    assert default_writer.qos.property[property_name] == "4096"

    qos = shared_participant.default_datawriter_qos
    other_writer = dds.DataWriter(shared_participant, topic, qos)
    assert other_writer.qos.property[property_name] == "4096"
    # We don't modify the input parameter
    assert not qos.property.exists(property_name)

    # If the property has been set, we honor the input value
    qos.property[property_name] = "3421"
    configured_writer = dds.DataWriter(shared_participant, topic, qos)
    assert configured_writer.qos.property[property_name] == "3421"

    # For a bounded type we don't change the qos settings
    assert not idl.get_type_support(Point).is_unbounded
    bounded_topic = dds.Topic(shared_participant, "test_bounded", Point)
    bounded_writer = dds.DataWriter(shared_participant, bounded_topic)
    assert not bounded_writer.qos.property.exists(property_name)


def test_unbounded_reader_qos(shared_participant: dds.DomainParticipant):
    property_name = 'dds.data_reader.history.memory_manager.fast_pool.pool_buffer_max_size'
    assert not idl.get_type_support(SequenceTest).is_keyed

    topic = dds.Topic(shared_participant, "test_unbounded", SequenceTest)
    sub = dds.Subscriber(shared_participant)
    unkeyed_unbounded_reader = dds.DataReader(sub, topic)

    # Unkeyed readers don't need unbounded Qos settings
    assert not unkeyed_unbounded_reader.qos.property.exists(property_name)

    assert idl.get_type_support(KeyedString).is_keyed
    topic = dds.Topic(shared_participant, "test_keyed_unbounded", KeyedString)
    keyed_unbounded_reader = dds.DataReader(sub, topic)
    assert keyed_unbounded_reader.qos.property[property_name] == "4096"

    # If the property has been set, we honor the input value
    qos = shared_participant.default_datareader_qos
    qos.property[property_name] = "3421"
    configured_reader = dds.DataReader(sub, topic, qos)
    assert configured_reader.qos.property[property_name] == "3421"

    # Test with CFT constructor
    cft = dds.ContentFilteredTopic(topic, "cft", "key = '1'")
    keyed_unbounded_reader = dds.DataReader(sub, cft)
    assert keyed_unbounded_reader.qos.property[property_name] == "4096"
    configured_reader = dds.DataReader(sub, cft, qos)
    assert configured_reader.qos.property[property_name] == "3421"
