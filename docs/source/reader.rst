.. py:currentmodule:: rti.connextdds

Subscriptions
~~~~~~~~~~~~~

To subscribe to DDS data you need a :class:`Subscriber` and a `DataReader` for
a ``Topic``.

A ``Subscriber`` and a ``DataReader`` can be configured with specific QoS and
a listener to receive status updates.

The following code creates a :class:`DynamicData.DataReader` for the topic
we created in the :ref:`topic:Topics` section.

.. code-block:: python

    subscriber = dds.Subscriber(participant)
    reader = dds.DynamicData.DataReader(subscriber, topic)

To receive the data call ``read()`` or ``take()``, which return a collection of
loaned data samples:

.. code-block:: python

    with reader.take() as samples:
        for sample in samples:
            if sample.info.valid:
                print(sample)

To get notified when new data is available you can use a :class:`StatusCondition`
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
