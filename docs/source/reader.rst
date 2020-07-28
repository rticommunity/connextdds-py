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

If you would like to use a :class:`WaitSet` and a :class:`StatusCondition`,
you can follow the example below:

.. code-block:: python

    samples_read = 0
    def handler(_):
        nonlocal samples_read
        nonlocal reader
        samples_read += len(reader.take())

    status_condition = dds.StatusCondition(reader)
    status_condition.enable_statuses = dds.StatusMask.data_available()
    status_condition.handler(handler)
    waitset = dds.WaitSet()
    waitset += status_condition 
    while samples_read < 10:
        print("Still looping")
        waitset.dispatch(dds.Duration(1)) # Wait 1 second each loop

The code block above will essentially print "Still looping" until 10 samples 
are processed.
