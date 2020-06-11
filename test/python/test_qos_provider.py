import rti.connextdds as dds


def test_string_uri_creation():
    assert len(dds.QosProvider("../xml/XmlApplication.xml").qos_profile_libraries()) == 4


def test_file_uri_creation():
    pass


def test_invalid_creation():
    pass


def test_default_provider():
    pass


def test_getters():
    pass
