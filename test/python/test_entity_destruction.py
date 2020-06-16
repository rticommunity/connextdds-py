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
    pass


def helper_close_participant_contained_entities(implicit_pub, test_retain):
    pass


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
