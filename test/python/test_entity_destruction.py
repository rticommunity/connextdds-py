import rti.connextdds as dds
import utils


def helper_close_subscriber_contained_entities(implicit_sub):
    p = utils.create_participant()
    
    topic1 = dds.StringTopicType.Topic(p, "test topic1")
    topic2 = dds.StringTopicType.Topic(p, "test topic2")
    topic3 = dds.StringTopicType.Topic(p, "test topic3")

    sub1 = None
    sub2 = dds.Subscriber(p)
    if not implicit_sub:
        sub1 = dds.Subscriber(p)
    else:
        sub1 = p.implicit_subscriber

    reader10 = dds.StringTopicType.DataReader(sub1, topic1)
    reader11 = dds.StringTopicType.DataReader(sub1, topic1)
    reader20 = dds.StringTopicType.DataReader(sub1, topic1)
    reader30 = dds.StringTopicType.DataReader(sub1, topic3)
    reader31 = dds.StringTopicType.DataReader(sub2, topic3)

    topic_query1 = dds.TopicQuery(
        dds.AnyDataReader(reader10),
        dds.TopicQuerySelection(dds.Filter("foo = 1 or foo = 3")),
    )
    topic_query2 = dds.TopicQuery(
        dds.AnyDataReader(reader31),
        dds.TopicQuerySelection(dds.Filter("foo = 4 or foo = 6")),
    )
    sub1.close()

    assert reader10.closed
    assert reader11.closed
    assert reader20.closed
    assert reader30.closed
    assert topic_query1.closed
    assert not reader31.closed
    assert not topic_query2.closed


def helper_close_publisher_contained_entities(implicit_pub):
    p = utils.create_participant()
    topic1 = dds.StringTopicType.Topic(p, "test topic1")
    topic2 = dds.StringTopicType.Topic(p, "test topic2")
    topic30 = dds.StringTopicType.Topic(p, "test topic3")

    pub1 = None
    pub2 = dds.Publisher(p)
    if not implicit_pub:
        pub1 = dds.Publisher(p)
    else:
        pub1 = p.implicit_publisher

    writer10 = dds.StringTopicType.DataWriter(pub1, topic1)
    writer11 = dds.StringTopicType.DataWriter(pub1, topic1)
    writer20 = dds.StringTopicType.DataWriter(pub1, topic2)
    writer30 = dds.StringTopicType.DataWriter(pub1, topic30)
    writer31 = dds.StringTopicType.DataWriter(pub2, topic30)
    
    pub1.close()

    assert pub1.closed
    assert not pub2.closed

    assert writer10.closed
    assert writer11.closed
    assert writer20.closed 
    assert writer30.closed
    assert not writer31.closed


def helper_close_participant_contained_entities(implicit_pub, test_retain):
    p1 = utils.create_participant()
    p2 = utils.create_participant()
    topic1 = dds.StringTopicType.Topic(p1, "test_topic1")
    topic2 = dds.StringTopicType.Topic(p1, "test topic2")
    topic30 = dds.StringTopicType.Topic(p1, "test topic3")
    topic31 = dds.StringTopicType.Topic(p2, "test topic3")
    cft10 = dds.StringTopicType.ContentFilteredTopic(topic1, "cft10", dds.Filter("value = '1'"))
    cft11 = dds.StringTopicType.ContentFilteredTopic(topic1, "cft11", dds.Filter("value = '1'"))
    cft20 = dds.StringTopicType.ContentFilteredTopic(topic31, "cft20", dds.Filter("value = '1'"))

    pub1 = None
    pub2 = None
    sub1 = None
    sub2 = None

    if not implicit_pub:
        pub1 = dds.Publisher(p1)
        pub2 = dds.Publisher(p2)
        sub1 = dds.Subscriber(p1)
        sub2 = dds.Subscriber(p2)
    else:
        pub1 = p1.implicit_publisher
        pub2 = p2.implicit_publisher 
        sub1 = p1.implicit_subscriber 
        sub2 = p2.implicit_subscriber 

    writer10 = dds.StringTopicType.DataWriter(pub1, topic1)
    writer11 = dds.StringTopicType.DataWriter(pub1, topic1)
    writer20 = dds.StringTopicType.DataWriter(pub1, topic2)
    writer30 = dds.StringTopicType.DataWriter(pub1, topic30)
    writer31 = dds.StringTopicType.DataWriter(pub2, topic31)
    
    reader10 = dds.StringTopicType.DataReader(sub1, topic1)
    reader11 = dds.StringTopicType.DataReader(sub1, topic2)
    reader20 = dds.StringTopicType.DataReader(sub1, cft10)
    reader30 = dds.StringTopicType.DataReader(sub1, topic30)
    reader31 = dds.StringTopicType.DataReader(sub2, topic31)
    reader32 = dds.StringTopicType.DataReader(sub2, cft20)

    flow_controller10 = dds.FlowController(p1, "fc10")
    flow_controller11 = dds.FlowController(p1, "fc11")
    flow_controller12 = dds.FlowController(p1, "fc12")
    flow_controller20 = dds.FlowController(p2, "fc20")

    tq1 = dds.TopicQuery(reader10, dds.TopicQuerySelection(dds.Filter("value = '1' or value = '3'")))
    tq2 = dds.TopicQuery(reader31, dds.TopicQuerySelection(dds.Filter("value = '4' or value = '6'")))
    
    if test_retain:
        pub1.retain()
        reader11.retain()
        writer11.retain()
        topic2.retain()
        cft11.retain()
        flow_controller12.retain()
        tq1.retain()

    flow_controller12.close()
    p1.close()

    assert p1.closed
        
    assert pub1.closed
    assert sub1.closed
    assert not pub2.closed
    assert not sub2.closed

    assert topic1.closed
    assert topic2.closed
    assert topic30.closed
    assert not topic31.closed

    assert cft10.closed
    assert cft11.closed
    assert not cft20.closed

    assert writer10.closed
    assert writer11.closed
    assert writer20.closed
    assert writer30.closed
    assert not writer31.closed

    assert reader10.closed
    assert reader11.closed
    assert reader20.closed
    assert reader30.closed
    assert not reader31.closed 

    assert flow_controller10.closed
    assert flow_controller11.closed
    assert flow_controller12.closed
    assert not flow_controller20.closed

    assert tq1.closed
    assert not tq2.closed

def test_close_subscriber_contained_entities():
    helper_close_subscriber_contained_entities(False)


def test_close_publisher_contained_entities():
    helper_close_publisher_contained_entities(False)


def test_close_participant_contained_entities():
    helper_close_participant_contained_entities(False, False)


def test_close_implicit_subscriber_contained_entities():
    helper_close_subscriber_contained_entities(True)


def test_close_implicit_publisher_contained_entities():
    helper_close_publisher_contained_entities(True)


def test_close_contained_entities_using_implicit_pub_sub():
    helper_close_participant_contained_entities(True, False)


def test_close_retained_contained_entities():
    helper_close_participant_contained_entities(False, True)


def test_close_datareader_contained_entity():
    def helper(retain):
        p = utils.create_participant()
        topic = dds.StringTopicType.Topic(p, "test_topic")
        sub = dds.Subscriber(p)
        reader = dds.StringTopicType.DataReader(sub, topic)
        topic_query = dds.TopicQuery(
            dds.AnyDataReader(reader),
            dds.TopicQuerySelection(dds.Filter("foo = 1 or foo = 3")),
        )

        if retain:
            topic_query.retain()
        reader.close()
        assert topic_query.closed
        assert reader.closed

    helper(False)
    helper(True)
