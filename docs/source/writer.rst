.. py:currentmodule:: rti.connextdds

Publications
~~~~~~~~~~~~

An application uses *DataWriters* to send data. A *DataWriter*
is associated with a single *Topic*. You can have multiple
*DataWriters* and *Topics* in a single application. In addition,
you can have more than one *DataWriter* for a particular *Topic*
in a single application. *Publishers* own and manage *DataWriters*.

To create a *DataWriter*, you need a *Topic* (see :ref:`topic:Topics`)
and a :ref:`participant:DomainParticipant`. Additionally,
you may add a QoS parameter and a listener.

The following code creates a :class:`DynamicData.DataWriter` for the *Topic*
we created in the :ref:`topic:Topics` section:

.. code-block:: python

    publisher = dds.Publisher(participant)
    writer = dds.DynamicData.DataWriter(publisher, topic)

To publish data, create a sample, update its values, and write it:

.. code-block:: python

    data = dds.DynamicData(my_type)
    data["foo"] = "test"
    writer.write(data)
