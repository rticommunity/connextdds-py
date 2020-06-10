import rti.connextdds as dds

def test_close_datareader_contained_entity():
    def helper(retain):
        p = dds.DomainParticipant(0, dds.DomainParticipantQos())
        topic = dds.StringTopicType.Topic(p, "test_topic")
        sub = dds.Subscriber(p)
        reader = dds.StringTopicType.DataReader(sub, topic)
        topic_query = dds.TopicQuery(dds.AnyDataReader(reader),
                                     dds.TopicQuerySelection(dds.Filter("foo = 1 or foo = 3")))

        #if (retain):
        #    topic_query.retain() <------ need retain()
        reader.close()
        assert topic_query.closed
        assert reader.closed


    helper(False)
    helper(True)