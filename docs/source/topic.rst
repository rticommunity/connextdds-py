.. py:currentmodule:: rti.connextdds

Topics
~~~~~~

Shared knowledge of the data types is a requirement for different
applications to communicate with DDS. The applications must also
share a way to identify what data is to be shared. Data (regardless of
type) is uniquely distinguished by using a name called a *Topic*.

*Topics* allow for *Publishers* and *Subscribers* to communicate
without knowing about each other. They dynamically discover
each other through *Topics*. The data that the :class:`DataReader` and
:class:`DataWriter` share is described by a *Topic*.

*Topics* are identified by a name, and they are associated with a type and a
:class:`DomainParticipant`.

The following code creates a *Topic* named "Example" for the type previously
defined in :ref:`types:Data Types`.

.. code-block:: python

    topic = dds.DynamicData.Topic(participant, "Example", my_type)

A `DataReader` can also be created with a ContentFilteredTopic
(see :class:`DynamicData.ContentFilteredTopic`), which specifies a content-based
subscription with a filter on the data type.
