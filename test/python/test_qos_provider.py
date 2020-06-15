import rti.connextdds as dds
import pytest
import pathlib

LOCATION = str(pathlib.Path(__file__).parent.absolute()) + "/"


def test_string_uri_creation():
    assert (
        len(
            dds.QosProvider(
                LOCATION + "../xml/XmlApplication.xml"
            ).qos_profile_libraries()
        )
        == 4
    )


def test_file_uri_creation():
    pass


def test_invalid_creation():
    with pytest.raises(dds.Error):
        dds.QosProvider("invalid_file.xml")
    with pytest.raises(dds.Error):
        dds.QosProvider(LOCATION + "../xml/InvalidXml.xml")


def test_default_provider():
    provider1 = dds.QosProvider.default()
    provider2 = dds.QosProvider.default()
    provider3 = provider2
    provider4 = provider3
    assert 3 == len(provider1.qos_profile_libraries())
    assert 3 == len(provider2.qos_profile_libraries())
    assert 3 == len(provider3.qos_profile_libraries())
    assert 3 == len(provider4.qos_profile_libraries())

    params = dds.QosProviderParams()
    params.url_profile = dds.StringSeq([LOCATION + "../xml/XmlApplication.xml"])
    provider4.reload_profiles()
    provider4.provider_params = params

    assert 4 == len(provider1.qos_profile_libraries())
    assert 4 == len(provider2.qos_profile_libraries())
    assert 4 == len(provider3.qos_profile_libraries())
    assert 4 == len(provider4.qos_profile_libraries())


def test_getters():
    pass
