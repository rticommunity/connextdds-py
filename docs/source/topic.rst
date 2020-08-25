.. py:currentmodule:: rti.connextdds

Topics
~~~~~~

DDS Topics allow for publishers and subscriber to communicate
without knowing about each other. They dynamically discover
each other through topics. The data that the :class:`DataReader` and
:class:`DataWriter` share is described by a topic.

Topics are identified by a name and they are associated to a type and a
:class:`DomainParticipant`.

The following code creates a topic named "Example" for the type previously
defined in :ref:`types:Data Types`.

.. code-block:: python

    topic = dds.DynamicData.Topic(participant, "Example", my_type)

A `DataReader` can also be created with a `ContentFilteredTopic`
(see :class:`DynamicData.ContentFilteredTopic`), which specifies a content-based
subscription with a filter on the data type.
