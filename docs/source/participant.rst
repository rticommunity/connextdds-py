.. py:currentmodule:: rti.connextdds

Participant
~~~~~~~~~~~

When using connextdds-py, one of the first things you'll need is to 
make a participant object.  To create a participant, all you need to 
do is import the module and then create the object. This can be seen
in the code below.
.. code-block:: python

    import rti.connextdds as dds 
    DOMAIN_ID = 0 # Could be anything, I just chose 0
    participant = dds.DomainParticipant(DOMAIN_ID)

Optionally, you can also create a participant using a with statement.
.. code-block:: python

    import rti.connextdds as dds 
    DOMAIN_ID = 0
    with dds.DomainParticipant(DOMAIN_ID) as participant:
        pass

Additionally, you can supply an additional parameter to specify the qos.
For example, if you would like to have a participant with a quicker 
shutdown cleanup period, you can follow the code below.
.. code-block:: python

    import rti.connextdds as dds 
    DOMAIN_ID = 0
    qos = dds.DomainParticipantQos()
    qos.database.shutdown_cleanup_period = dds.Duration.from_milliseconds(10)
    participant = dds.DomainParticipant(DOMAIN_ID, qos)
    



