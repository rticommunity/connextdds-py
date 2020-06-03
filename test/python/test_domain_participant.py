import rti.connextdds as dds 
import pytest

DOMAIN_ID = 0

#def test_participant_qos_value_type():
#    pass

#def test_participant_factory_qos_value_type():
#    pass

def test_participant_default_creation():
    p = dds.DomainParticipant(DOMAIN_ID)
    assert p.domain_id == DOMAIN_ID

#def test_participant_creation_w_default_qos():
#    p = dds.DomainParticipant(DOMAIN_ID, dds.DomainParticipantQos())
#    assert p.domain_id = DOMAIN_ID
#    default_qos << p.qos.policy
#    default_qos.domain_participant_resource_limits.writer_property_string_max_length = 

#def test_participant_creation_w_qos():
#    pass

def test_participant_creation_w_listener():
    l = dds.NoOpDomainParticipantListener()
    p = dds.DomainParticipant(DOMAIN_ID, dds.DomainParticipantQos(), l)
    assert p.listener == l
    p.close()

def test_participant_creation_failure():
    with pytest.raises(dds.Error):
        qos = dds.DomainParticipantQos()
        qos.domain_participant_resource_limits.type_object_max_serialized_length = -2
        dds.DomainParticipant(0, qos)

#def test_set_get_qos():
#    pass
    
#def test_set_qos_exception():
#    pass
    
#def test_set_get_default_qos():
#    pass
    
#def test_set_get_factory_qos():
#    pass

#def test_set_get_listener():
#    pass

#def test_reference_assignment():
#    pass

#def test_find():
#    pass

#def test_close():
#    pass
    
#def test_already_closed_exception():
#    pass

#def test_retain():
#    pass

#def test_creation_from_native():
#    pass

#def test_current_time():
#    pass
    
#def test_assert_liveliness():
#    pass

#def test_ignore():
#    pass

#def test_add_peer():
#    pass

#def test_remove_peer():
#   pass

#def test_domain_participant_config_params():
#    pass

#def test_find_extensions():
#    pass

#def test_entity_lock():
#    pass

#def test_retain_for_listener():
#    pass

#def test_discovered_participants():
#    pass

#def test_dns_polling_period():
#    pass

#def test_participant_factory_qos_to_string():
#    pass

#def test_domain_participant_qos_to_string():
#    pass
