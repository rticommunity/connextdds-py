.. py:currentmodule:: rti.connextdds

Topics
~~~~~~

Shared knowlege of the data types is a requirement for different
applications to communicate with this API. THe applications must also
share a way to identify what data is to be shared. Data (regardless of
type) is uniquelu distinguished by using a name called a Topic.
By definition, a Topic corresponds to a single data type. However,
several Topics may refer to the same data type.

DDS Topics allow for publishers and subscriber to communicate
without knowing about each other. They dynamically discover
each other through topics. The data that the :class:`DataReader` and 
:class:`DataWriter` share is described by a topic. 

To create a topic, all you need to do is create a topic object.
Due to the dynamic nature of Python, dynamic data types are used.
To create a dynamic topics, you just need a domain participant
and a data type. How to create a type and participant is
discussed in :ref:`types:Types` and :ref:`participant:DomainParticipant`.

.. code-block:: python

    import rti.connextdds as dds
    # Assuming that provider_type has you type and 
    # participant has your participant
    topic = dds.DynamicData.Topic(participant, "Name of topic", provider_type)



