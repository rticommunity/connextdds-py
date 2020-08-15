.. py:currentmodule:: rti.connextdds

Topics
~~~~~~

Shared knowledge of the data types is a requirement for different
applications to communicate with DDS. The applications must also
share a way to identify what data is to be shared. Data (regardless of
type) is uniquely distinguished by using a name called a Topic.

Topics allow for publishers and subscriber to communicate
without knowing about each other. They dynamically discover
each other through Topics. The data that the :class:`DataReader` and 
:class:`DataWriter` share is described by a :class:`Topic`. 

To create a Topics, you need a domain participant
and a data type. How to create a type and participant is
discussed in :ref:`types:Types` and :ref:`participant:DomainParticipant`.

.. code-block:: python

    import rti.connextdds as dds
    # my_type and participant have already been created
    topic = dds.DynamicData.Topic(participant, "Name of topic", my_type)



