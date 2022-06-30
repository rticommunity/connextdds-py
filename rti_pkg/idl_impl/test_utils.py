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

import os
import time

from typing import Any, Optional, Union
from dataclasses import fields, MISSING
from enum import IntEnum
import inspect
import rti.idl as idl
import rti.idl_impl.reflection_utils as reflection_utils
from rti.idl_impl.reflection_utils import get_args
from rti.idl_impl.type_plugin import TypeSupportKind
from rti.idl_impl.type_utils import ListFactory, PrimitiveArrayFactory, ValueListFactory, get_optimal_collection_factory
from itertools import islice, cycle
import rti.idl_impl.annotations as annotations
from rti.idl_impl.unions import union_cases

import rti.connextdds as dds

"""Test utilities for IDL-based user data types.

Provides utilities to create test data samples for any given type and publish
and subscribe to them.
"""

# --- User type test utilities ------------------------------------------------


class IdlValueGenerator:
    """Class for creating data samples with test values given an IDL type"""

    def __init__(self, sample_type: type, max_string_bounds=1000, max_seq_bounds=10, keys_only=False):
        """Creates an IdlValueGenerator for the given type"""

        self.sample_type = sample_type
        self.type_support = idl.get_type_support(sample_type)
        self.dynamic_type = self.type_support.dynamic_type
        self.max_string_bounds = max_string_bounds
        self.max_seq_bounds = max_seq_bounds
        self.keys_only = keys_only

    def create_test_data(self, seed: int = 0) -> Any:
        """Returns an instance of value of the type given to the constructor.

        This function is deterministic and will always return the same value
        given the same seed.
        """

        return self._create_test_data_impl(self.sample_type, seed)

    def _get_sequence_type_bounds_annotation(self, seq_name: str):
        """Private member function used to determine a sequences max length"""
        # You must check that the type is a sequence before calling this
        member_annotations = self.type_support.member_annotations.get(
            seq_name, [])
        array_annotation = annotations.find_annotation(
            member_annotations, annotations.ArrayAnnotation)
        if array_annotation.is_array:
            return array_annotation

        bound_annotation = annotations.find_annotation(
            member_annotations, annotations.BoundAnnotation)
        return bound_annotation

    def _get_sequence_type_length(self, seq_name: str, seed: int):
        """Private member function used to determine the length of a sequence"""
        bound = self._get_sequence_type_bounds_annotation(seq_name)
        if isinstance(bound, annotations.ArrayAnnotation):
            return bound.total_size
        return seed % bound.value % self.max_seq_bounds

    def _get_str_sequence_type_bounds(self, seq_name: str):
        """Private member function used to determine a string sequences max length"""
        # You must check that the type is a str sequence before calling this

        if self.dynamic_type[seq_name].type.kind == dds.TypeKind.ARRAY_TYPE:
            seq_bounds = self.dynamic_type[seq_name].type.total_element_count
        else:
            seq_bounds = self.dynamic_type[seq_name].type.bounds

        str_bounds = self.dynamic_type[seq_name].type.content_type.bounds

        return (seq_bounds, str_bounds)

    def _get_str_sequence_type_length(self, seq_name: str, seed: int):
        """Private member function used to determine the length of a string sequence"""
        seq_length = self._get_sequence_type_length(seq_name, seed)
        element_annotations = annotations.find_annotation(
            self.type_support.member_annotations.get(seq_name),
            annotations.ElementAnnotations)
        str_bound = annotations.find_annotation(
            element_annotations.value, annotations.BoundAnnotation)

        if str_bound == idl.unbounded:
            str_length = seed % self.max_string_bounds
        else:
            str_length = (seed % str_bound.value) % self.max_string_bounds

        return (seq_length, str_length)

    def _generate_string_from_seed(self, bounds: int, seed: int) -> str:
        """Private local function used to generate string values"""

        if bounds > self.max_string_bounds:
            bounds = self.max_string_bounds

        if seed % 4 == 0:
            # return a single character, could be any capital ascii char
            return chr((seed % 26) + 65)
        elif seed % 4 == 1 or bounds == 0:
            return ""
        elif seed % 4 == 2:
            return "".join([chr((x % 26) + 65) for x in range(0, bounds)])
        else:
            return "".join([chr((x % 26) + 65) for x in range(0, seed % bounds)])

    def _generate_enum_from_seed(self, enum_type: type, seed: int):
        """Private local function used to generate enum values"""
        return next(islice(cycle(enum_type), seed, seed + 1))

    def _generate_seq_from_seed(self, sample_type: type, member_type: type, field_name: str, field_factory, keys_only, seed: int):
        """Private local function used to generate sequence values"""
        if field_factory not in [None, list, MISSING] and isinstance(field_factory, (ListFactory, PrimitiveArrayFactory, ValueListFactory)):
            return field_factory()

        if reflection_utils.get_underlying_type(member_type) is str:
            return self._generate_str_seq_from_seed(field_name, field_factory, seed)

        length = self._get_sequence_type_length(field_name, seed)
        seq = [self._generate_value_from_seed(sample_type, reflection_utils.get_underlying_type(
            member_type), field_name, None, seed + x, keys_only) for x in range(0, length)]
        return self._make_sequence_from_list(field_factory, seq)

    def _generate_str_seq_from_seed(self, field_name: str, field_factory, seed: int):
        """Private local function used to generate string sequence values"""
        seq_length, str_length = self._get_str_sequence_type_length(
            field_name, seed)
        return self._make_sequence_from_list(
            field_factory,
            [self._generate_string_from_seed(str_length, seed + x) for x in range(0, seq_length)])

    def _generate_union_from_seed(self, sample_type: type, seed: int):
        union_case_list = union_cases(sample_type)
        index = seed % len(union_case_list)
        value_name = union_case_list[index].name
        value_type = get_args(union_case_list[index].type)[0]
        if reflection_utils.is_constructed_type(value_type):
            value_value = IdlValueGenerator(
                value_type).create_test_data(seed)
        else:
            value_value = self._generate_value_from_seed(
                sample_type,
                value_type,
                value_name,
                self._get_field_factory(union_case_list[index]),
                seed)
        union = sample_type()

        # upon setting the attribute the discriminator is set properly
        setattr(union, value_name, value_value)
        return union

    def _generate_value_from_seed(self, sample_type: type, member_type: type, field_name: str, field_factory, seed: int, keys_only=False):
        """Recursive private function used to generate the values given a type"""

        if member_type is bool:
            return seed % 2 == 0
        elif reflection_utils.is_primitive(member_type):
            # return mod 128 so it fits in all types
            return member_type(seed % 128)
        elif member_type is str:
            bound_annotation = self._get_sequence_type_bounds_annotation(
                field_name)
            if isinstance(bound_annotation, annotations.ArrayAnnotation):
                bound = bound_annotation.total_size
            else:
                bound = bound_annotation.value
            return self._generate_string_from_seed(bound, seed)
        elif reflection_utils.is_enum(member_type):
            return self._generate_enum_from_seed(member_type, seed)
        elif reflection_utils.is_optional_type(member_type):
            # If the seed is even we will fill the optional type
            if seed % 2 == 0:
                return self._generate_value_from_seed(
                    sample_type, reflection_utils.get_underlying_type(member_type), field_name, field_factory, seed, self.keys_only)
            else:
                return None
        elif reflection_utils.is_sequence_type(member_type):
            return self._generate_seq_from_seed(sample_type, member_type, field_name, field_factory, self.keys_only, seed)
        elif reflection_utils.is_constructed_type(member_type):
            # Here there is a special case if we are only generating keys. If
            # the inner type has keys and this field is a key then we only want
            # to generate those, else generate the whole structure
            generate_keys = keys_only and has_keys(
                member_type) and is_field_key(sample_type, field_name)
            return IdlValueGenerator(
                member_type,
                max_seq_bounds=self.max_seq_bounds,
                max_string_bounds=self.max_string_bounds,
                keys_only=generate_keys)._create_test_data_impl(
                    member_type, seed)
        else:
            raise TypeError(
                f"unknown type {str(member_type)} given to generator")

    def _create_test_data_impl(self, sample_type: type = None, seed: int = 0) -> Any:
        """Private local function used to generate the values given an idl type"""
        if sample_type is None:
            sample_type = self.sample_type

        if reflection_utils.is_enum(sample_type):
            # Enums are a special case because they do not have a default ctor
            return self._generate_enum_from_seed(sample_type, seed)

        if idl.get_type_support(sample_type).kind == TypeSupportKind.UNION:
            return self._generate_union_from_seed(sample_type, seed)

        value = sample_type()

        for field in fields(sample_type):
            if self.keys_only:
                # A member is part of the key if annotated or when the type
                # doesn't have any keys(the whole sample is the key)
                is_key = not has_keys(sample_type) or is_field_key(
                    sample_type, field.name)
                if not is_key:
                    continue
            val = self._generate_value_from_seed(
                sample_type,
                field.type,
                field.name,
                self._get_field_factory(field),
                seed,
                keys_only=self.keys_only)
            if val is not None or reflection_utils.is_optional_type(field.type):
                # The only time val will return None is if the type is optional
                # or if it is a keyed type with nested keys
                setattr(value, field.name, val)
            seed += 1
        return value

    def _make_sequence_from_list(self, field_factory, lst):
        """Private function used to create a sequence from a list using its factory"""
        # will return either a list or an array depending on the factory
        if field_factory in [list, None, MISSING] or isinstance(field_factory, ListFactory):
            return lst
        else:
            arr = field_factory()
            for i in range(0, len(arr)):
                arr[i] = lst[i]
            return arr

    def _is_alias_type(self) -> bool:
        """Private function used to check if this is an alias"""
        return isinstance(self.dynamic_type, dds.AliasType)

    def _get_field_factory(self, field):
        ts = idl.get_type_support(self.sample_type)
        member_annotations = ts.member_annotations.get(field.name, [])
        if field.default_factory is not MISSING:
            return field.default_factory

        field_type = reflection_utils.remove_classvar(field.type)
        if reflection_utils.is_optional_type(field_type):
            field_type = reflection_utils.get_underlying_type(field_type)

        if reflection_utils.is_primitive_or_enum(field_type):
            return None
        if field_type is str:
            return None

        if reflection_utils.is_sequence_type(field_type):
            # In the Python type there is no distinction between an array
            # and a sequence; the ArrayAnnotation makes it an array.
            array_info = annotations.find_annotation(
                member_annotations, annotations.ArrayAnnotation)
            element_type = reflection_utils.get_underlying_type(field_type)
            dimensions = array_info.dimensions if array_info.is_array else 0
            return get_optimal_collection_factory(
                element_type,
                dimensions)

        if reflection_utils.is_constructed_type(field_type):
            return field_type


def is_field_key(t: type, field_name: str) -> bool:
    """Function used to check if a field is a key"""

    type_support = idl.get_type_support(t)
    member_annotations = type_support.member_annotations.get(
        field_name, [])
    key_annotation = annotations.find_annotation(
        member_annotations, annotations.KeyAnnotation)
    return key_annotation.value


def has_keys(t: Any) -> bool:
    """Function used to check if this type has a key"""
    for field in fields(t):
        if is_field_key(t, field.name):
            return True
    return False


def keys_equal(a: Any, b: Any) -> bool:
    """Function used to determine if two values have the same keys"""
    if type(a) is not type(b):
        return False

    def key_equal_helper(a: Any, b: Any) -> bool:
        type_has_keys = has_keys(type(a))
        for field in fields(type(a)):
            if reflection_utils.is_constructed_type(field.type) and not reflection_utils.is_enum(field.type):
                if has_keys(getattr(a, field.name)) or is_field_key(type(a), field.name):
                    if not key_equal_helper(getattr(a, field.name), getattr(b, field.name)):
                        return False
            is_key = not type_has_keys or is_field_key(type(a), field.name)
            if is_key:
                if getattr(a, field.name) != getattr(b, field.name):
                    return False
        return True

    return key_equal_helper(a, b)

# --- Wait test utilitites -----------------------------------------------------


class WaitError(Exception):
    def __init__(self, message: str = None):
        if message is None:
            super().__init__("Wait timeout")
        else:
            super().__init__("Wait timeout: " + str(message))


class Wait:
    def until(self, predicate) -> None:
        max_wait = 10
        sleep_time = 0.1
        while not predicate() and max_wait > 0:
            time.sleep(sleep_time)
            max_wait -= sleep_time

        if max_wait <= 0:
            raise WaitError()

    def until_equal(self, expected_value, function) -> None:
        try:
            self.until(lambda: expected_value == function())
        except WaitError:
            raise WaitError(f"Expected {expected_value}, got {function()}")

    def for_discovery(self, reader: dds.DataReader, writer: dds.DataWriter) -> None:
        self.until(lambda: writer.instance_handle in reader.matched_publications)
        self.until(lambda: reader.instance_handle in writer.matched_subscriptions)

    def for_data(self, reader: dds.DataReader, count: int = -1) -> None:
        if count < 0:
            # Wait for one or more samples
            self.until(lambda: reader.datareader_cache_status.sample_count > 0)
        else:
            # Wait for a specific number
            self.until_equal(
                count, lambda: reader.datareader_cache_status.sample_count)

    def for_samples(self, reader: dds.DataReader, count: int = -1) -> None:
        if count < 0:
            # Wait for one or more samples
            self.until(lambda: len(reader.read()) > 0)
        else:
            # Wait for a specific number
            self.until_equal(count, lambda: len(reader.read()))


wait = Wait()

# --- Pub/sub test utilities --------------------------------------------------

# We subtract 1024 from the max serialized size because the core does some
# operations on the returned result that may end up adding some additional byte
_MAX_SERIALIZED_SIZE_ADDITIONAL_BYTES = 1024
_UNBOUNDED_LENGTH = idl.unbounded.value - _MAX_SERIALIZED_SIZE_ADDITIONAL_BYTES


def get_test_domain():
    return int(os.environ.get('TEST_DOMAIN', 0))


def get_test_participant_qos():
    qos = dds.DomainParticipantQos()
    qos.database.shutdown_cleanup_period = dds.Duration.from_milliseconds(10)
    return qos


def create_participant():
    return dds.DomainParticipant(get_test_domain(), get_test_participant_qos())


def create_topic(participant: dds.DomainParticipant, type: type, topic_name: Optional[str] = None):
    name = f'Example {type}' if topic_name is None else topic_name
    # TODO PY-21: these helpers wouldn't be necessary if all topics were
    # unified under dds.Topic.

    # For non-IDL types, such as dds.StringTopicType, dds.DynamicData:
    if not _is_idl_type(type):
        if type.__class__ in (dds.StructType, dds.UnionType, dds.DynamicData):
            return dds.DynamicData.Topic(participant, name, type)
        else:
            return type.Topic(participant, name)

    # For IDL types:
    else:
        return dds.Topic(participant, name, type)

# Gets the "namespace" that contains this topic. When type is an IDL type,
# it returns the dds package; for the rest it returns the type, e.g. if
# type is a StringTopicType, it returns dds.StringTopicType.


def _get_topic_namespace(topic, type):
    if isinstance(topic, dds.Topic) or isinstance(topic, dds.ContentFilteredTopic):
        return dds
    elif isinstance(topic, dds.DynamicData.Topic):
        return dds.DynamicData
    else:
        return type


def _create_reader(topic, type, reader_qos):
    ns = _get_topic_namespace(topic, type)
    return ns.DataReader(topic.participant, topic, reader_qos)


def _create_writer(topic, type, writer_qos):
    ns = _get_topic_namespace(topic, type)
    return ns.DataWriter(topic.participant, topic, writer_qos)


def _is_idl_type(type):
    return inspect.getmodule(type).__name__ != "rti.connextdds"


class PubSubFixture:
    def __init__(
        self,
        participant,
        data_type,
        topic=None,
        topic_name=None,
        create_writer=True,
        create_reader=True,
        writer_policies=[],
        reader_policies=[],
        content_filter=None
    ):
        self.data_type = data_type

        # Figure out if the type has an unbounded value in it
        if not _is_idl_type(data_type):
            has_unbound_member = False
        else:
            type_support = idl.get_type_support(data_type)
            has_unbound_member = _UNBOUNDED_LENGTH <= type_support.max_serialized_sample_size

        self.participant = participant or create_participant()
        self.topic = topic or create_topic(
            self.participant, data_type, topic_name)

        if create_writer:
            writer_qos = self.participant.implicit_publisher.default_datawriter_qos
            writer_qos << dds.Reliability.reliable()
            writer_qos << dds.History.keep_all
            for policy in writer_policies:
                writer_qos << policy
            if has_unbound_member and \
                    not writer_qos.property.exists('dds.data_writer.history.memory_manager.fast_pool.pool_buffer_max_size'):
                writer_qos.property['dds.data_writer.history.memory_manager.fast_pool.pool_buffer_max_size'] = "1000"
            self.writer = _create_writer(self.topic, data_type, writer_qos)

        if create_reader:
            reader_qos = self.participant.implicit_subscriber.default_datareader_qos
            reader_qos << dds.Reliability.reliable()
            reader_qos << dds.History.keep_all
            for policy in reader_policies:
                reader_qos << policy
            if has_unbound_member and \
                    not reader_qos.property.exists('dds.data_reader.history.memory_manager.fast_pool.pool_buffer_max_size'):
                reader_qos.property['dds.data_reader.history.memory_manager.fast_pool.pool_buffer_max_size'] = "1000"
            if content_filter is not None:
                cft_name = "Filtered " + self.topic.name
                if type(content_filter) == str:
                    cft = dds.ContentFilteredTopic(
                        self.topic, cft_name, dds.Filter(content_filter))
                elif type(content_filter) == tuple:
                    cft = dds.ContentFilteredTopic(
                        self.topic, cft_name, dds.Filter(*content_filter))
                else:
                    raise TypeError(
                        f"Expected str or tuple, given {type(content_filter)}")
                self.reader = _create_reader(cft, data_type, reader_qos)
            else:
                self.reader = _create_reader(self.topic, data_type, reader_qos)

        if create_reader and create_writer:
            wait.for_discovery(self.reader, self.writer)

    def send_and_check(self, data):
        self.writer.write(data)
        self.check_data(data)

    def check_data(self, data):
        if type(data) is not list:
            data = [data]

        wait.for_data(self.reader, count=len(data))
        received_data = self.reader.take_data()
        assert len(received_data) == len(data)
        assert all(d in received_data for d in data)
