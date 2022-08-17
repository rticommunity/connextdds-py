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

The following code creates a *Topic* named "Car Position" for a type ``Point``:

.. code-block:: python

    topic = dds.Topic(participant, "Car Position", Point)

Where ``Point`` can be defined in IDL as:

.. code-block:: idl

    struct Point {
        int64 x;
        int64 y;
    };

And in Python as follows:

.. code-block:: python

    import rti.types as idl

    @idl.struct
    class Point:
        x: int = 0
        y: int = 0

:ref:`types:Data Types` explains how to define your types in more detail.

Special Topics
--------------

In addition to the class :class:`Topic`, there are a few separate Topic classes
for certain types:

* For ``DynamicData`` topics: :class:`DynamicData.Topic` (see :ref:`types:DynamicType and DynamicData`)
* For the built-in Discovery Topics: :class:`ParticipantBuiltinTopicData.Topic`, :class:`SubscriptionBuiltinTopicData.Topic`, :class:`PublicationBuiltinTopicData.Topic`, :class:`TopicBuiltinTopicData.Topic`

