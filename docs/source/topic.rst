.. py:currentmodule:: rti.connextdds

Topic
~~~~~

DDS Topics allow for publishers and subscriber to communicate
without knowing about each other. They dynamically discover
each other through topics. The data that the DataReader and 
DataWriter share is described by a topic. 

To create a topic, all you need to do is create a topic object.
Due to the dynamic nature of Python, dynamic data types are used.
To create a dynamic topics, you just need a domain participant
and a data type. How to create a type and participant is
discussed in :ref:`types:Types` and :ref:`participant:Participant`.

.. code-block:: python

    import rti.connextdds as dds
    # Assuming that provider_type has you type and 
    # participant has your participant
    topic = dds.DynamicData.Topic(participant, "Name of topic", provider_type)



