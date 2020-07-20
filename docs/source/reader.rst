.. py:currentmodule:: rti.connextdds

Reader
~~~~~~

Creating a data reader in connextdds-py is super simple!
All you need is create a :ref:`topic:Topic` and a :ref:`participant:Participant`.

.. code-block:: python
    
    # At this point you should have a participant and a 
    # topic already created. Instructions on this can be
    # found in the documentation
    # First, you need a subscriber in order to create a reader
    import rti.connextdds as dds 
    subscriber = dds.Subscriber(participant)
    reader = dds.DynamicData.DataReader(subscriber, topic)

To use the reader, you have a few options, the simplest of them
looks like this:

.. code-block:: python

    samples = reader.take()
    for sample in samples:
        if sample.info.valid:
            print(sample)

