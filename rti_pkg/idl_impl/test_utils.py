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
from typing import Any, Optional
from dataclasses import fields
import rti.idl as idl
import rti.idl_impl.reflection_utils as reflection_utils
from rti.idl_impl.type_plugin import PrimitiveArrayFactory

import rti.connextdds as dds

"""Test utilities for IDL-based user data types.

Provides utilities to create test data samples for any given type and publish
and subscribe to them.
"""

# --- User type test utilities ------------------------------------------------

# Set the maximum length a string can be from the generator
MAX_BOUNDS = 1000


class IdlValueGenerator:
    """Class for creating data samples with test values given an IDL type"""

    def __init__(self, sample_type: type):
        """Creates an IdlValueGenerator for the given type"""

        self.sample_type = sample_type
        self.dynamic_type = idl.get_type_support(sample_type).dynamic_type

    def create_test_data(self, seed: int = 0) -> Any:
        """Returns an instance of value of the type given to the constructor.

        This function is deterministic and will always return the same value
        given the same seed.
        """

        return self._create_test_data_impl(self.sample_type, seed)

    def _create_test_data_impl(self, sample_type: type = None, seed: int = 0) -> Any:

        if sample_type is None:
            sample_type = self.sample_type

        def _generate_string_from_seed(bounds: int, seed: int) -> str:
            """Private local function used to generate string values"""

            if bounds > MAX_BOUNDS:
                bounds = MAX_BOUNDS

            if seed % 4 == 0:
                # return a single character, could be any printable ascii char
                return chr((seed % 94) + 33)
            elif seed % 4 == 1:
                return ""
            elif seed % 4 == 2:
                return "".join([chr((x % 94) + 33) for x in range(0, bounds)])
            else:
                return "".join([chr((x % 94) + 33) for x in range(0, seed % bounds)])

        def _generate_value_from_seed(sample_type: type, member_type: type, field_name: str, field_factory, seed: int):
            """Recursive private function used to generate the values given a type"""

            if member_type is bool:
                return seed % 2 == 0
            elif reflection_utils.is_primitive(member_type):
                # return mod 128 so it fits in all types
                return member_type(seed % 128)
            elif member_type is str:
                bound = self._get_sequence_type_bounds(field_name)
                return _generate_string_from_seed(bound, seed)
            elif reflection_utils.is_optional_type(member_type):
                # If the seed is even we will fill the optional type
                if seed % 2 == 0:
                    return _generate_value_from_seed(
                        sample_type, reflection_utils.get_underlying_type(member_type), field_name, None, seed)
                else:
                    return None
            elif reflection_utils.is_sequence_type(member_type):
                # is either a list or primitive array
                bound = self._get_sequence_type_bounds(field_name)
                if bound == idl.unbounded.value:
                    length = seed
                else:
                    length = (seed % bound) + 1

                seq = [_generate_value_from_seed(sample_type, reflection_utils.get_underlying_type(
                    member_type), "", None, seed + x) for x in range(0, length)]
                if type(field_factory) == PrimitiveArrayFactory:
                    arr = field_factory()
                    arr.fromlist(seq)
                    return arr
                else:
                    return seq
            elif reflection_utils.is_constructed_type(member_type):
                return self._create_test_data_impl(member_type, seed)
            else:
                raise TypeError(
                    f"unknown type {str(member_type)} given to generator")

        value = sample_type()

        for field in fields(sample_type):
            setattr(value, field.name, _generate_value_from_seed(
                sample_type, field.type, field.name, field.default_factory, seed))
            seed += 1
        return value

    def _get_sequence_type_bounds(self, seq_name: str):
        """Private member function used to determine a sequences max length"""

        # You must check that the type is a sequence before calling this
        return self.dynamic_type[seq_name].type.bounds


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
            self.until_equal(count, lambda: reader.datareader_cache_status.sample_count)

wait = Wait()

# --- Pub/sub test utilitites --------------------------------------------------

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
    if "dds." in str(type):
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
    else:
        return type


def _create_reader(topic, type, reader_qos):
    ns = _get_topic_namespace(topic, type)
    return ns.DataReader(topic.participant, topic, reader_qos)


def _create_writer(topic, type, writer_qos):
    ns = _get_topic_namespace(topic, type)
    return ns.DataWriter(topic.participant, topic, writer_qos)


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
        if "dds." in str(data_type):
            has_unbound_member = False
        else:
            type_support = idl.get_type_support(data_type)
            has_unbound_member = _UNBOUNDED_LENGTH <= type_support.max_serialized_sample_size

        if participant is None:
            self.participant = create_participant()
        else:
            self.participant = participant
        self.participant = participant or create_participant()
        self.topic = topic or create_topic(
            self.participant, data_type, topic_name)

        if create_writer:
            writer_qos = self.participant.implicit_publisher.default_datawriter_qos
            writer_qos << dds.Reliability.reliable()
            writer_qos << dds.History.keep_all
            for policy in writer_policies:
                writer_qos << policy
            if has_unbound_member:
                writer_qos.property['dds.data_writer.history.memory_manager.fast_pool.pool_buffer_max_size'] = str(
                    1000)
            self.writer = _create_writer(self.topic, data_type, writer_qos)

        if create_reader:
            reader_qos = self.participant.implicit_subscriber.default_datareader_qos
            reader_qos << dds.Reliability.reliable()
            reader_qos << dds.History.keep_all
            for policy in reader_policies:
                reader_qos << policy
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
