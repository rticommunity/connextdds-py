.. py:currentmodule:: rti.connextdds

DomainParticipant
~~~~~~~~~~~~~~~~~

*DomainParticipants* are the focal point for creating, destroying, and managing
other *Connext DDS* objects. A DDS domain is a logical network of applications:
only applications that belong to the same DDS domain may communicate using
*Connext DDS*. A DDS domain is identified by a unique integer value known as a
domain ID. An application participates in a DDS domain by creating a
*DomainParticipant* for that domain ID.

The following code creates a :class:`DomainParticipant` on domain `0`.

.. code-block:: python

    import rti.connextdds as dds
    participant = dds.DomainParticipant(domain_id=0)


Like all :class:`IEntity` types, *DomainParticipants* have QoS policies and
listeners. The following example shows how to create a *DomainParticipant*
with a specific QoS policy:

.. code-block:: python

    qos = dds.DomainParticipantQos()
    qos.database.shutdown_cleanup_period = dds.Duration.from_milliseconds(10)
    participant = dds.DomainParticipant(domain_id=0, qos=qos)


A *DomainParticipant* and its contained entities can also be created from an XML
definition with the :meth:`QosProvider.create_participant_from_config` function.

*DomainParticipants* (and all other *Entities*) get destroyed automatically
when they are garbage collected; however to ensure that they are destroyed at a
certain point in your application, you can call ``close()`` or create them
within a ``with`` block:

.. code-block:: python

    with dds.DomainParticipant(domain_id=0) as participant:
        print(participant.domain_id)
        # ...


