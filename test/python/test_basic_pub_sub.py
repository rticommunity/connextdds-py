import rti.connextdds as dds
import time


def test_data_reader_writer():
    participant = dds.DomainParticipant(0)
    topic = dds.StringTopicType.Topic(participant, "pub_sub_example")
    # Create qos for both, add reliable and keep all
    writer_qos = participant.implicit_publisher.default_datawriter_qos
    writer_qos << dds.Reliability.reliable()
    writer_qos << dds.History.keep_all()
    writer_qos << dds.Durability.transient_local()
    
    # Create the reader qos
    reader_qos = participant.implicit_subscriber.default_datareader_qos
    reader_qos << dds.Reliability.reliable()
    reader_qos << dds.History.keep_all()
    reader_qos << dds.Durability.transient_local()
    writer = dds.StringTopicType.DataWriter(
        participant.implicit_publisher, topic, writer_qos
    )
    reader = dds.StringTopicType.DataReader(
        participant.implicit_subscriber, topic, reader_qos
    )
    # Need to have reader check for writer and vice versa here
    # writer.wait_for_acknowledgments(1000)
    count = 0

    for i in range(1, 6):
        writer.write("Hello World " + str(i))
    while len(reader.read()) < 5:
        time.sleep(0.001)
    for x in reader.read():
        count += 1
        print(str(x))

    assert count == 5
