.. py:currentmodule:: rti.connextdds

Subscriptions
~~~~~~~~~~~~~

An application uses *DataReaders* to access data received over DDS.
A *DataReader* is associated with a single *Topic*. You can have
multiple *DataReaders* and *Topics* in a single application. In
addition, you can have more than one *DataReader* for a particular
*Topic* in a single application. *Subscribers* own and manage *DataReaders*.

To create a *DataReader*, you need to create a *Topic* (see :ref:`topic:Topics`) and
a :ref:`participant:DomainParticipant`. Optionally, you can add
a QoS parameter and a listener.

The following code creates a :class:`DynamicData.DataReader` for the *Topic*
we created in the :ref:`topic:Topics` section:

.. code-block:: python

    subscriber = dds.Subscriber(participant)
    reader = dds.DynamicData.DataReader(subscriber, topic)

To receive the data, call ``read()`` or ``take()``, which return a collection of
loaned data samples:

.. code-block:: python

    with reader.take() as samples:
        for sample in samples:
            if sample.info.valid:
                print(sample.data)

To get notified when new data is available, you can use a :class:`StatusCondition`
and a :class:`WaitSet`:

.. code-block:: python

    samples_read = 0
    def handler(_):
        nonlocal reader
        with reader.take() as samples:
            for sample in samples:
                if sample.info.valid:
                    print(sample)

    # Each Entity has a StatusCondition
    status_condition = dds.StatusCondition(reader)

    # Specify which status to get notified about and set the handler:
    status_condition.enable_statuses = dds.StatusMask.data_available()
    status_condition.handler(handler)

    # Attach the condition to a waitset and call dispatch() to execute the
    # condition handlers when they become active
    waitset = dds.WaitSet()
    waitset += status_condition
    while True:
        waitset.dispatch(dds.Duration(4)) # Wait up to 4 seconds

Alternatively, you can use a `DataReaderListener` to get notified
(see :class:`DynamicData.DataReaderListener`).
