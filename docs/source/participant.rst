.. py:currentmodule:: rti.connextdds

DomainParticipant
~~~~~~~~~~~~~~~~~

DomainParticipants are the focal point for creating, destroying, and managing
other Connext DDS objects. A DDS domain is a logical network of applications:
only applications that belong to the same DDS domain may communicate using
Connext DDS. A DDS domain is identified by a unique integer value known as a
domain ID. An application participates in a DDS domain by creating a
DomainParticipant for that domain ID.

The following code creates a :class:`DomainParticipant` on domain `0`.

.. code-block:: python

    import rti.connextdds as dds
    DOMAIN_ID = 0
    participant = dds.DomainParticipant(DOMAIN_ID)

You can also create a participant using a :code:`with` statement so it gets
deleted when the block ends.

.. code-block:: python

    import rti.connextdds as dds
    DOMAIN_ID = 0
    with dds.DomainParticipant(DOMAIN_ID) as participant:
        pass

Like all :class:`IEntity` types, DomainParticipants have QoS policies and listeners.

The following code shows how to specify the :class:`DomainParticipantQos`:

.. code-block:: python

    import rti.connextdds as dds
    DOMAIN_ID = 0
    qos = dds.DomainParticipantQos()
    qos.database.shutdown_cleanup_period = dds.Duration.from_milliseconds(10)
    participant = dds.DomainParticipant(DOMAIN_ID, qos)


A DomainParticpant and its contained entities can also be created from an XML
definition with :meth:`QosProvider.create_participant_from_config` function.


