.. py:currentmodule:: rti.connextdds

Subscriptions
~~~~~~~~~~~~~

To create a DataReader, you need is create a :ref:`topic:Topics` and 
a :ref:`participant:DomainParticipant`. Optionally, you can add
a QoS parameter and a listener.

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

