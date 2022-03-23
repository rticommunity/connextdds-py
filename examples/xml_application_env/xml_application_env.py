"""
 (c) 2020 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 RTI grants Licensee a license to use, modify, compile, and create derivative
 works of the Software.  Licensee has the right to distribute object form only
 for use with RTI products.  The Software is provided "as is", with no warranty
 of any type, including any warranty for fitness for any purpose. RTI is under
 no obligation to maintain or support the Software.  RTI shall not be liable for
 any incidental or consequential damages arising out of the use or inability to
 use the software.
 """

import rti.connextdds as dds
import os
import random
import time


QOS_URL = "file://application.xml"
PARTICIPANT_NAME = "domain_participant_library::participant"
WRITER_NAME = "publisher::writer"
READER_NAME = "subscriber::reader"
KVP_TYPE_NAME = "KeyValuePair"


class MyDataReaderListener(dds.DynamicData.NoOpDataReaderListener):
    def on_data_available(self, reader):
        with reader.take() as samples:
            for (data, info) in samples:
                if info.valid:
                    print(data)
                else:
                    print("  Received metadata")


def publish_env(id, writer, kvp_type, env):
    sample = dds.DynamicData(kvp_type)
    sample["id"] = id

    for var, value in env.items():
        sample["key"] = var
        sample["value"] = value
        writer.write(sample)


if __name__ == "__main__":
    qos_provider = dds.QosProvider(QOS_URL)

    participant = qos_provider.create_participant_from_config(PARTICIPANT_NAME)

    my_type = qos_provider.type(qos_provider.type_libraries[0], KVP_TYPE_NAME)

    writer = dds.DynamicData.DataWriter.find_by_name(participant, WRITER_NAME)

    reader = dds.DynamicData.DataReader.find_by_name(participant, READER_NAME)
    reader.set_listener(MyDataReaderListener(), dds.StatusMask.DATA_AVAILABLE)

    participant.ignore_participant(participant.instance_handle)

    id = random.randint(0, 2147483647)

    while True:
        print("Publishing Environment")
        publish_env(id, writer, my_type, os.environ)
        time.sleep(2)
