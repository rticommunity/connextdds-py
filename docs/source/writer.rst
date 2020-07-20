.. py:currentmodule:: rti.connextdds

Writer
~~~~~~

Creating a data writer in the Python API couldn't be easier!
All you have to do is create a :ref:`topic:Topic` and a :ref:`participant:Participant`.

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

