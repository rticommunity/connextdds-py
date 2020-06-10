import rti.connextdds as dds 

class TestSystem:
    def __init__(self, the_participant):
        self.participant = the_participant 
        self.topic = dds.StringTopicType.Topic(self.participant, "TestType")
        self.reader = dds.StringTopicType.DataReader(self.participant, self.topic)
        self.writer = dds.StringTopicType.DataWriter(self.participant, self.topic)
