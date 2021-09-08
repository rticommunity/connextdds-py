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

import rti.connextdds as dds
import rti.request as request
import pytest
import queue
import time

TEST_DOMAIN_ID = 100

def get_keyed_string_dynamic_type():
    keyed_string_struct = dds.StructType('DynamicKeyedString')
    keyed_string_struct.add_member(dds.Member('key', dds.StringType(), is_key=True))
    keyed_string_struct.add_member(dds.Member('value', dds.StringType()))
    return keyed_string_struct


def create_dynamic_rr_data(k, v):
    data = dds.DynamicData(get_keyed_string_dynamic_type())
    data['key'] = k
    data['value'] = v
    return data


def parse_dynamic_rr_data(data):
    return data['key'], data['value'] 


def create_rr_data(k, v):
    data = dds.KeyedStringTopicType()
    data.key = k
    data.value = v
    return data


def parse_rr_data(data):
    return data.key, data.value
    

def create_dynamic_topic(participant, name):
    return dds.DynamicData.Topic(participant, name, get_keyed_string_dynamic_type())


def create_topic(participant, name):
    return dds.KeyedStringTopicType.Topic(participant, name)


def copy_dynamic_sample(loaned_sample):
    return dds.DynamicData.Sample(loaned_sample)


def copy_sample(loaned_sample):
    return dds.KeyedStringTopicType.Sample(loaned_sample)


class RequestReplyTester:
    def __init__(self, domain_id, use_dynamic_data, use_qos_object, use_custom_topic, use_replier_cft, use_pub_sub_args, use_callback_funcs, use_simple_replier):
        qos = dds.DomainParticipantQos()
        qos.database.shutdown_cleanup_period = dds.Duration.from_milliseconds(100)
        self._participant = dds.DomainParticipant(domain_id, qos)
        if use_dynamic_data:
            self._type = get_keyed_string_dynamic_type()
            self.create_data_fnc = create_dynamic_rr_data
            self.parse_data_fnc = parse_dynamic_rr_data
            self.copy_sample_fnc = copy_dynamic_sample
            create_topic_func = create_dynamic_topic
        else:
            self._type = type_cls = dds.KeyedStringTopicType
            self.create_data_fnc = create_rr_data
            self.parse_data_fnc = parse_rr_data
            self.copy_sample_fnc = copy_sample
            create_topic_func = create_topic

        service_name = 'Test'

        if use_qos_object:
            request_writer_qos = request.Requester.default_writer_qos
            request_reader_qos = request.Replier.default_reader_qos
            reply_writer_qos = request.Replier.default_writer_qos
            reply_reader_qos = request.Requester.default_reader_qos
        else:
            request_writer_qos = request_reader_qos = reply_writer_qos = reply_reader_qos = None
        
        if use_custom_topic:
            # Test with custom Topic object
            request_topic = create_topic_func(self._participant, 'TestRequest')
            # Test with custom string Topic name
            reply_topic = 'TestReply'
        else:
            request_topic = reply_topic = None

        if use_replier_cft:
            replier_filter = dds.Filter("NOT (key MATCH 'filtered')")
            request_reader_topic = create_topic_func(self._participant, 'TestRequest') if request_topic is None else request_topic
            if isinstance(self._type, dds.DynamicType):
                request_reader_topic = dds.DynamicData.ContentFilteredTopic(request_reader_topic, 'request_cft', replier_filter)
            else:
                request_reader_topic = dds.KeyedStringTopicType.ContentFilteredTopic(request_reader_topic, 'request_cft', replier_filter)
        else:
            request_reader_topic = request_topic

        if use_pub_sub_args:
            publisher = dds.Publisher(self._participant)
            subscriber = dds.Subscriber(self._participant)
        else:
            publisher = subscriber = None

        self._cb = use_callback_funcs
        self._request_queue = queue.Queue()
        self._reply_queue = queue.Queue()
        if use_callback_funcs:
            def replier_callback(replier):
                for sample in (s for s in replier.take_requests() if s.info.valid):
                    self._request_queue.put(self.copy_sample_fnc(sample))
                
            def requester_callback(requester):
                for sample in (s for s in requester.take_replies() if s.info.valid):
                    self._reply_queue.put(self.copy_sample_fnc(sample))
        else:
            replier_callback = requester_callback = None

        self._requester = request.Requester(
            self._type,
            self._type,
            self._participant,
            service_name,
            request_topic,
            reply_topic,
            request_writer_qos,
            reply_reader_qos,
            publisher,
            subscriber,
            requester_callback
        )

        self._simple = use_simple_replier
        if use_simple_replier:
            def simple_replier_callback(request):
                key, value = self.parse_data_fnc(request)
                reply = self.create_data_fnc(key, value + ' reply')
                return reply
            self._replier = request.SimpleReplier(
                self._type,
                self._type,
                self._participant,
                simple_replier_callback,
                service_name,
                request_reader_topic,
                reply_topic,
                reply_writer_qos,
                request_reader_qos,
                publisher,
                subscriber
            )
        else:
            self._replier = request.Replier(
                self._type,
                self._type,
                self._participant,
                service_name,
                request_reader_topic,
                reply_topic,
                reply_writer_qos,
                request_reader_qos,
                publisher,
                subscriber,
                replier_callback
            )


    def close(self):
        self.requester.close()
        self.replier.close()
        self.participant.close()
    
    
    def wait_for_requests(self, count, timeout):
        waited_time = 0
        sleep_interval = timeout / 10.0
        requests = []
        if self._simple:
            raise dds.IllegalOperationError('Simple Replier cannot wait for requests')
        if (self._cb):
            while waited_time < timeout and len(requests) < count:
                while not self._request_queue.empty():
                    requests.append(self._request_queue.get())
                if len(requests) < count:
                    time.sleep(sleep_interval)
                    waited_time += sleep_interval
        else:
            if self._replier.wait_for_requests(float(timeout), count):
                with self._replier.take_requests() as loaned_samples:
                    for sample in (s for s in loaned_samples if s.info.valid):
                        requests.append(self.copy_sample_fnc(sample))

        if len(requests) < count:
            raise dds.TimeoutError('Did not receive expected request count within timeout')
        
        return requests


    def wait_for_replies(self, id, count, timeout):
        waited_time = 0
        sleep_interval = timeout / 10.0
        replies = []
        if (self._cb):
            while waited_time < timeout and len(replies) < count:
                while not self._reply_queue.empty():
                    reply = self._reply_queue.get()
                    if request.Requester.is_related_reply(id, reply.info):
                        replies.append(reply)
                if len(replies) < count:
                    time.sleep(sleep_interval)
                    waited_time += sleep_interval
        else:
            if self._requester.wait_for_replies(timeout, count):
                with self._requester.take_replies(id) as loaned_samples:
                    for sample in (s for s in loaned_samples if s.info.valid):
                        replies.append(self.copy_sample_fnc(sample))

        if len(replies) < count:
            raise dds.TimeoutError('Did not receive expected reply count within timeout')
        
        return replies


    def wait_for_discovery(self):
        while self.requester.matched_replier_count == 0 and self.replier.matched_requester_count == 0:
            time.sleep(0.01)


    def create_data(self, key, value):
        return self.create_data_fnc(key, value)

    
    def parse_data(self, data):
        return self.parse_data_fnc(data)
    

    @property
    def requester(self):
        # type: () -> request.Requester
        return self._requester


    @property
    def replier(self):
        return self._replier

    @property
    def participant(self):
        return self._participant


@pytest.mark.parametrize('use_dynamic_data', (True, False))
@pytest.mark.parametrize('use_qos_object', (True, False))
@pytest.mark.parametrize('use_custom_topic', (True, False))
@pytest.mark.parametrize('use_replier_cft', (True, False))
@pytest.mark.parametrize('use_pub_sub_args', (True, False))
@pytest.mark.parametrize('use_callback_funcs', (True, False))
def test_request_single_reply(
        use_dynamic_data,
        use_qos_object,
        use_custom_topic,
        use_replier_cft,
        use_pub_sub_args,
        use_callback_funcs):
    test_object = RequestReplyTester(
        TEST_DOMAIN_ID,
        use_dynamic_data,
        use_qos_object,
        use_custom_topic,
        use_replier_cft,
        use_pub_sub_args,
        use_callback_funcs,
        False)

    test_object.wait_for_discovery()

    if use_replier_cft:
        filtered_key = 'filtered'
        filtered_val = 'filtered_data'
        req = test_object.create_data(filtered_key, filtered_val)
        test_object.requester.send_request(req)

    key = 'request1'
    val = 'request'
    req = test_object.create_data(key, val)
    id = test_object.requester.send_request(req)
    requests = test_object.wait_for_requests(1, 1.0)
    assert len(requests) == 1
    reply_key, reply_val = test_object.parse_data(requests[0].data)
    test_object.replier.send_reply(test_object.create_data(reply_key, reply_val + ' reply'), requests[0].info)
    replies = test_object.wait_for_replies(id, 1, 1.0)
    assert len(replies) == 1
    assert request.Requester.is_final_reply(replies[0])
    assert request.Requester.is_final_reply(replies[0].info)
    reply_key, reply_val = test_object.parse_data(replies[0].data)
    assert reply_key == key
    assert reply_val == val + ' reply'
    test_object.requester.request_datawriter.dispose_instance(req)
    test_object.requester.request_datawriter.unregister_instance(req)
    test_object.replier.reply_datawriter.dispose_instance(req)
    test_object.replier.reply_datawriter.unregister_instance(req)
    test_object.close()


@pytest.mark.parametrize('use_dynamic_data', (True, False))
@pytest.mark.parametrize('use_qos_object', (True, False))
@pytest.mark.parametrize('use_custom_topic', (True, False))
@pytest.mark.parametrize('use_replier_cft', (True, False))
@pytest.mark.parametrize('use_pub_sub_args', (True, False))
@pytest.mark.parametrize('use_callback_funcs', (True, False))
def test_request_simple_reply(
        use_dynamic_data,
        use_qos_object,
        use_custom_topic,
        use_replier_cft,
        use_pub_sub_args,
        use_callback_funcs):
    test_object = RequestReplyTester(
        TEST_DOMAIN_ID,
        use_dynamic_data,
        use_qos_object,
        use_custom_topic,
        use_replier_cft,
        use_pub_sub_args,
        use_callback_funcs,
        True)

    test_object.wait_for_discovery()

    if use_replier_cft:
        filtered_key = 'filtered'
        filtered_val = 'filtered_data'
        req = test_object.create_data(filtered_key, filtered_val)
        test_object.requester.send_request(req)

    key = 'request1'
    val = 'request'
    req = test_object.create_data(key, val)
    id = test_object.requester.send_request(req)
    replies = test_object.wait_for_replies(id, 1, 1.0)
    assert len(replies) == 1
    reply_key, reply_val = test_object.parse_data(replies[0].data)
    assert reply_key == key
    assert reply_val == val + ' reply'
    test_object.close()


asyncio = pytest.importorskip("asyncio")

class RequestReplyTesterAsync(RequestReplyTester):
    def __init__(self, domain_id, use_dynamic_data, use_qos_object, use_custom_topic, use_replier_cft, use_pub_sub_args):
        super().__init__(domain_id, use_dynamic_data, use_qos_object, use_custom_topic, use_replier_cft, use_pub_sub_args, False, False)

    async def wait_for_requests_async(self, count, timeout):
        requests = []
        if await self._replier.wait_for_requests_async(float(timeout), count):
            with self._replier.take_requests() as loaned_samples:
                for sample in (s for s in loaned_samples if s.info.valid):
                    requests.append(self.copy_sample_fnc(sample))

        if len(requests) < count:
            raise dds.TimeoutError('Did not receive expected request count within timeout')
        
        return requests


    async def wait_for_replies_async(self, id, count, timeout):
        replies = []
        if await self._requester.wait_for_replies_async(timeout, count):
            with self._requester.take_replies(id) as loaned_samples:
                for sample in (s for s in loaned_samples if s.info.valid):
                    replies.append(self.copy_sample_fnc(sample))

        if len(replies) < count:
            raise dds.TimeoutError('Did not receive expected reply count within timeout')
        
        return replies


async def request_reply_async(
        use_dynamic_data,
        use_qos_object,
        use_custom_topic,
        use_replier_cft,
        use_pub_sub_args):
    test_object = RequestReplyTesterAsync(
        TEST_DOMAIN_ID,
        use_dynamic_data,
        use_qos_object,
        use_custom_topic,
        use_replier_cft,
        use_pub_sub_args)

    test_object.wait_for_discovery()

    if use_replier_cft:
        filtered_key = 'filtered'
        filtered_val = 'filtered_data'
        req = test_object.create_data(filtered_key, filtered_val)
        await test_object.requester.send_request_async(req)

    key = 'request1'
    val = 'request'
    req = test_object.create_data(key, val)
    id = await test_object.requester.send_request_async(req)
    requests = await test_object.wait_for_requests_async(1, 1.0)
    assert len(requests) == 1
    reply_key, reply_val = test_object.parse_data(requests[0].data)
    test_object.replier.send_reply(test_object.create_data(reply_key, reply_val + ' reply'), requests[0].info)
    replies = await test_object.wait_for_replies_async(id, 1, 1.0)
    assert len(replies) == 1
    assert request.Requester.is_final_reply(replies[0])
    assert request.Requester.is_final_reply(replies[0].info)
    reply_key, reply_val = test_object.parse_data(replies[0].data)
    assert reply_key == key
    assert reply_val == val + ' reply'
    await test_object.requester.request_datawriter.dispose_instance_async(req)
    await test_object.requester.request_datawriter.unregister_instance_async(req)
    await test_object.replier.reply_datawriter.dispose_instance_async(req)
    await test_object.replier.reply_datawriter.unregister_instance_async(req)
    test_object.close()


@pytest.fixture
def event_loop():
    loop = asyncio.new_event_loop()
    yield loop
    loop.close()


@pytest.mark.parametrize('use_dynamic_data', (True, False))
@pytest.mark.parametrize('use_qos_object', (True, False))
@pytest.mark.parametrize('use_custom_topic', (True, False))
@pytest.mark.parametrize('use_replier_cft', (True, False))
@pytest.mark.parametrize('use_pub_sub_args', (True, False))
def test_request_reply_async(event_loop, use_dynamic_data, use_qos_object, use_custom_topic, use_replier_cft, use_pub_sub_args):
    event_loop.run_until_complete(request_reply_async(use_dynamic_data, use_qos_object, use_custom_topic, use_replier_cft, use_pub_sub_args))
