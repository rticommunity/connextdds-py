.. py:currentmodule:: rti.connextdds

Publications
~~~~~~~~~~~~

To create a DataWriter, you need a :ref:`topic:Topics` 
and a :ref:`participant:DomainParticipant`. Additionally,
you may add a QoS parameter.

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

