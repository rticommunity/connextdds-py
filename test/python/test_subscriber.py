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

from test_utils.fixtures import *
import rti.connextdds as dds
from rti.types.builtin import String


def test_subscriber_listener_can_be_set(shared_participant):
    listener = dds.NoOpSubscriberListener()
    other_listener = dds.NoOpSubscriberListener()
    sub = dds.Subscriber(shared_participant, dds.SubscriberQos(), listener)
    assert sub.listener == listener

    sub.listener = other_listener
    assert sub.listener == other_listener

    sub.set_listener(None, dds.StatusMask.NONE)
    assert sub.listener is None

    sub.listener = dds.NoOpSubscriberListener()
    assert type(sub.listener) == dds.NoOpSubscriberListener


def test_subscriber_find_readers(shared_participant):

    sub = dds.Subscriber(shared_participant)
    assert len(sub.find_datareaders()) == 0

    topic = dds.Topic(shared_participant, 'Foo', String)
    qos = dds.DataReaderQos()
    qos.entity_name = dds.EntityName('foo')
    reader = dds.DataReader(sub, topic, qos)
    assert len(sub.find_datareaders()) == 1
    reader2 = dds.DataReader(sub, topic)
    assert len(sub.find_datareaders()) == 2
    assert reader, reader2 in sub.find_datareaders()
    assert sub.find_datareader('foo') == reader
    # find_datareader_by_topic_names returns the first reader found
    # so it could be reader or reader2
    assert sub.find_datareader_by_topic_name('Foo') in (reader, reader2)

    assert reader, reader2 in sub.find_datareaders('Foo')
    assert reader, reader2 in sub.find_datareaders(dds.DataState.any())
    assert reader not in sub.find_datareaders('Bar')
    assert reader not in sub.find_datareaders(dds.DataState.new_data)


def test_subscriber_qos(shared_participant):
    sub = dds.Subscriber(shared_participant)
    assert sub.qos == dds.SubscriberQos()

    qos = dds.SubscriberQos()
    qos.partition.name = ['test']
    sub.qos = qos
    assert sub.qos == qos
    assert sub.qos.partition.name == ['test']


def test_subscriber_get_participant(shared_participant):
    sub = dds.Subscriber(shared_participant)
    assert sub.participant == shared_participant


def test_subscriber_default_datareader_qos(shared_participant):
    sub = dds.Subscriber(shared_participant)
    assert sub.default_datareader_qos == dds.DataReaderQos()
    dr_qos = dds.DataReaderQos()
    dr_qos.entity_name.name = 'dr_test'
    sub.default_datareader_qos = dr_qos
    assert sub.default_datareader_qos == dr_qos
    dr = dds.DataReader(sub, dds.Topic(shared_participant, 'Foo', String))
    assert dr.qos.entity_name.name == 'dr_test'


def test_notify_datareaders(shared_participant):
    class SubListener(dds.NoOpSubscriberListener):
        on_data_on_readers_called = False

        def on_data_on_readers(self, sub):
            self.on_data_on_readers_called = True

    class DRListener(dds.NoOpDataReaderListener):
        on_data_available_called = False

        def on_data_available(self, dr):
            self.on_data_available_called = True

    dr_listener = DRListener()
    sub_listener = SubListener()

    sub = dds.Subscriber(shared_participant, dds.SubscriberQos(), sub_listener)
    topic = dds.Topic(shared_participant, 'Foo', String)

    dr = dds.DataReader(sub, topic, dds.DataReaderQos(), dr_listener)
    dw = dds.DataWriter(dds.Publisher(shared_participant), topic)
    wait.for_discovery(dr, dw)

    dw.write(String('test'))
    wait.until(lambda: sub_listener.on_data_on_readers_called)
    assert not dr_listener.on_data_available_called

    sub.notify_datareaders()
    wait.until(lambda: dr_listener.on_data_available_called)
