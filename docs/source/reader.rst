.. py:currentmodule:: rti.connextdds

Subscriptions
~~~~~~~~~~~~~

An application uses *DataReaders* to access data received over DDS.
A *DataReader* is associated with a single *Topic*. You can have
multiple *DataReaders* and *Topics* in a single application. In
addition, you can have more than one *DataReader* for a particular
*Topic* in a single application. *Subscribers* own and manage *DataReaders*.

Creating a DataReader
---------------------

To create a *DataReader*, you need to create a *Topic* (see :ref:`topic:Topics`) and
a :ref:`participant:DomainParticipant`. Optionally, you can add
a QoS parameter and a listener.

The following code creates a :class:`DataReader` for the *Topic*
we created in the :ref:`topic:Topics` section:

.. code-block:: python

    subscriber = dds.Subscriber(participant)
    reader = dds.DataReader(subscriber, topic)

Reading data
------------

When data is available, the :meth:`DataReader.read_data` and
:meth:`DataReader.take_data` methods return a collection of data samples of the
type specified in the *Topic*. (``take_data`` removes the data from the reader,
and ``read_data`` keeps it so it can be accessed again.)

.. code-block:: python

    for sample in reader.take_data():
        print(sample)

To also access the meta-data associated with each data sample, use
:meth:`DataReader.take`  or :meth:`DataReader.read`:


.. code-block:: python

    for data, info in reader.take():
        if info.valid:
            print(f"Data received: {data}")
        else:
            print(f"State changed: {info.state}")

The :meth:`DataReader.select` method allows selecting which
data to read.

Being notified when data is available
-------------------------------------

There are a few ways to check if a reader has data available:

- Polling for data
- Using a Condition and a WaitSet
- Reading with an asynchronous generator
- Using the DataReaderListener

Polling for data means that you call the "read" or "take" methods described
before at certain intervals to check if they return any data.

A :class:`StatusCondition` and a :class:`WaitSet` allows waiting synchronously
until a DataReader status change triggers, including the ``DATA_AVAILABLE``
status:

.. code-block:: python

    def process_data(_):
        nonlocal reader
        for sample in reader.take_data():
            print(sample)

    # Each Entity has a StatusCondition
    status_condition = dds.StatusCondition(reader)

    # Specify which status to get notified about and set the handler:
    status_condition.enable_statuses = dds.StatusMask.DATA_AVAILABLE
    status_condition.handler(process_data)

    # Attach the condition to a waitset and call dispatch() to execute the
    # condition handlers when they become active
    waitset = dds.WaitSet()
    waitset += status_condition
    while True:
        waitset.dispatch(dds.Duration(4)) # Wait up to 4 seconds

The ``async`` versions of the "take" methods provide a simple way to write your
subscriber application. The methods :meth:`DataReader.take_data_async` or
:meth:`DataReader.take_async` work as asynchronous generators, returning
data as it is received and awaiting as necessary.

To use these functions your application must ``import rti.asyncio``, which
requires Python 3.7+.

.. code-block:: python

    import rti.asyncio
    # ...

    async def print_infinite(reader: dds.DataReader):
        # Print data as it arrives, suspending the coroutine until data is
        # available.
        async for data in reader.take_data_async():
            print(data)

    if __name__ == "__main__":
        # create reader...

        # you can use Python's asyncio.run() as well
        rti.asyncio.run(print_infinite(reader))

``take_data_async()`` and ``take_async()`` receive an optional ``condition``
argument (a ``dds.ReadCondition`` or ``dds.QueryCondition``) that can select
data by state or content.

Finally, you can use a :class:`DataReaderListener` to get notified of status
updates, including new data. This method is only recommended for lightweight
processing, since the listener callback is executed in an internal Connext
thread, and should not block or perform CPU-heavy operations.

.. note::
    **Differences between a regular DataReader and a DynamicData.DataReader**

    Read this note if your application uses ``dds.DynamicData`` for your for
    types.

    Unlike ``dds.DataReader``, ``dds.DynamicData.DataReader`` doesn't define
    ``take_data()``, ``read_data()``, ``take_data_async()``, or ``take_async()``.

    Also, when you use a ``dds.DynamicData.DataReader``, the objects returned by
    ``take()`` or ``read()`` are loaned; that is, they contain memory that is
    owned by the *DataReader*, and they can't be used after the collection of
    samples is destroyed.

    A normal ``dds.DataReader`` on the other hand returns new objects with no
    lifecycle restrictions.