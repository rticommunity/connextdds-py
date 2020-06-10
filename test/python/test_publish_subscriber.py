import rti.connextdds as dds
import utils
import time

DOMAIN_ID = 0

def test_write_timestamp():
    system = utils.TestSystem(dds.DomainParticipant(DOMAIN_ID)) 
    test_timestamp = dds.Time(123)
    system.writer.write("hi", test_timestamp) 
    time.sleep(2)
    samples = system.reader.take()
    assert samples[0].info.source_timestamp == test_timestamp

def test_write_handle():
    system = utils.TestSystem(dds.DomainParticipant(DOMAIN_ID))
    sample = dds.StringTopicType()
    handle = system.writer.register_instance(sample)
    system.writer.write(sample, handle)
    time.sleep(3)
    samples = system.reader.take()
    assert samples[0].info.instance_handle == handle

def test_write_handle_and_timestamp():
    pass


def test_write_topic_w_iterator():
    pass


def test_write_timestamp_w_iterator():
    pass


def test_write_handle_w_iterator():
    pass


def test_write_handle_and_timestamp_w_iterator():
    pass


def test_write_timestamp_w_operator():
    pass


def test_write_handle_w_operator():
    pass


def test_write_w_operator_mix():
    pass
