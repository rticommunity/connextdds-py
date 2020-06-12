import rti.connextdds as dds
import time


class TestSystem:
    def __init__(self, the_participant, sample_type):
        self.participant = the_participant
        if sample_type == "StringTopicType":
            self.topic = dds.StringTopicType.Topic(self.participant, "StringTopicType")
            self.reader = dds.StringTopicType.DataReader(self.participant, self.topic)
            self.writer = dds.StringTopicType.DataWriter(self.participant, self.topic)
        elif sample_type == "KeyedStringTopicType":
            self.topic = dds.KeyedStringTopicType.Topic(
                self.participant, "KeyedStringTopicType"
            )
            self.reader = dds.KeyedStringTopicType.DataReader(
                self.participant, self.topic
            )
            self.writer = dds.KeyedStringTopicType.DataWriter(
                self.participant, self.topic
            )
        else:
            raise Exception(sample_type + " not supported in test system")


def create_participant():
    qos = dds.DomainParticipantQos()
    qos.database.shutdown_cleanup_period = (0, 10000000)
    qos.database.shutdown_timeout = (0, 10000000)
    return dds.DomainParticipant(0, qos)


def wait(reader, t=10, count=1):
    curr = 0
    while t > curr:
        if len(reader.read()) >= count:
            break
        time.sleep(0.5)
        curr += 0.5
    else:
        raise Exception("Waited for " + str(t) + " seconds and recieved nothing")
