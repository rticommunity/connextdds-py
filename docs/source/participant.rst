.. py:currentmodule:: rti.connextdds

DomainParticipant
~~~~~~~~~~~~~~~~~

When using connextdds-py, one of the first things you'll need is to 
make a :class:`DomainParticipant` object.  To create a participant, all you need to 
do is import the module and then create the object. This can be seen
in the code below.

.. code-block:: python

    import rti.connextdds as dds 
    DOMAIN_ID = 0
    participant = dds.DomainParticipant(DOMAIN_ID)

Optionally, you can also create a participant using a :code:`with`
statement so it gets deleted when the block ends.

.. code-block:: python

    import rti.connextdds as dds 
    DOMAIN_ID = 0
    with dds.DomainParticipant(DOMAIN_ID) as participant:
        pass

Additionally, you can supply an additional parameter to specify the
:class:`DomainParticipantQos`.
For example, if you would like to have a participant with a quicker 
shutdown cleanup period, you can follow the code below.

.. code-block:: python

    import rti.connextdds as dds 
    DOMAIN_ID = 0
    qos = dds.DomainParticipantQos()
    qos.database.shutdown_cleanup_period = dds.Duration.from_milliseconds(10)
    participant = dds.DomainParticipant(DOMAIN_ID, qos)
    
Another way to create a participant is to use the 
:code:`QosProvider.create_participant_from_config` function. This will
create the participant and its entities from the XML definition.

