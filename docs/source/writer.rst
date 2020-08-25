.. py:currentmodule:: rti.connextdds

Publications
~~~~~~~~~~~~

To publish DDS data you need a :class:`Publisher` and a ``DataWriter`` for a
``Topic``.

A ``Publisher`` and a ``DataWriter`` can be configured with specific QoS and
a listener to receive status updates.

The following code creates a :class:`DynamicData.DataWriter` for the topic
we created in the :ref:`topic:Topics` section.

.. code-block:: python

    publisher = dds.Publisher(participant)
    writer = dds.DynamicData.DataWriter(publisher, topic)

To publish data, create a sample, update its values and write it:

.. code-block:: python

    data = dds.DynamicData(my_type)
    data["foo"] = "test"
    writer.write(data)
