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

The following code creates a :class:`DataWriter` for the *Topic*
we created in the :ref:`topic:Topics` section:

.. code-block:: python

    publisher = dds.Publisher(participant)
    writer = dds.DataWriter(publisher, topic)

To publish data, create a sample, set the values, and write it:

.. code-block:: python

    data = Point(x=1, y=2)
    writer.write(data)

A special DataWriter type for DynamicData, :class:`DynamicData.DataWriter` is
also available. Find more information in :ref:`types:DynamicType and DynamicData`.