.. py:currentmodule:: rti.connextdds

Publications
~~~~~~~~~~~~

An application uses DataWriters to send data. A DataWriter
is associated with a single Topic. You can have multiple
DataWriters and Topics in a single application. In addition,
you can have more than one DataWriter for a particular Topic
in a single application.

A Publisher is the DDS object responsible for the actual
sending of data. Publishers own and manage DataWriters. A
DataWriter can only be owned by a single Publisher while a
Publisher can own many DataWriters.

To create a DataWriter, you need a :ref:`topic:Topics` 
and a :ref:`participant:DomainParticipant`. Additionally,
you may add a QoS parameter and a listener.

.. code-block:: python
    
    # At this point you should have a participant and a 
    # topic already created. Instructions on this can be
    # found in the documentation
    # First, you need a publisher in order to create a writer
    import rti.connextdds as dds 
    publisher = dds.Publisher(participant)
    writer = dds.DynamicData.DataWriter(publisher, topic)

To use the writer, create some data. Once it is created, you
can call:

.. code-block:: python

    writer.write(data)

