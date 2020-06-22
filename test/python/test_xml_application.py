import rti.connextdds as dds
import utils
import pytest
import pathlib

FILE = (
    str(pathlib.Path(__file__).parent.absolute())
    + "/"
    + "../xml/AppCreationTestConfiguration.xml",)



def setup_qos_provider_create_participant(name):
    params = dds.DomainParticipantConfigParams(0)
    provider_params = dds.QosProviderParams()
    provider_params.url_profile = FILE

    dds.QosProvider.default_provider_params = provider_params 

    participant = dds.QosProvider(name).create_participant_from_config(name, params)
    assert not (participant is None)
    return participant

    
    

@pytest.mark.parametrize(
    "pub_p_name,sub_p_name,writer_name,reader_name,filtered",
    [
        (
            "TestParticipantLibrary::DynamicPublicationParticipant",
            "TestParticipantLibrary::DynamicSubscriptionParticipant",
            "XMLAppCreationTestWriter",
            "TestSubscriber::XMLAppCreationTestReader",
            False,
        )
    ],
)
def test_xml_app_pub_sub(pub_p_name, sub_p_name, writer_name, reader_name, filtered):
    pub_participant = setup_qos_provider_create_participant(pub_p_name)
    sub_participant = setup_qos_provider_create_participant(sub_p_name)

    writer = pub_participant.find_datawriter(writer_name)
    assert not (writer is None)

    reader = sub_participant.find_datareader(reader_name)
    assert not (reader is None)

    sample = range(1, 7)
    writer.write(sample)

    sample = range(1, 3)
    writer.write(sample)
    
    sample = range(1, 9)
    writer.write(sample)

    if filtered:
        utils.wait(reader, 10, 2)
        samples = reader.take()
        
        assert samples[0].data == range(1, 7)
        assert samples[1].data == range(1, 9)

    else:
        utils.wait(reader, 10, 3)
        samples = reader.take()
        
        assert samples[0].data == range(1, 7)
        assert samples[1].data == range(1, 3)
        assert samples[2].data == range(1, 9)

