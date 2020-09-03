.. py:currentmodule:: rti.connextdds

Introduction
~~~~~~~~~~~~

This is the Python API for *RTI® Connext® DDS*.

As an experimental product, this API documentation is less
comprehensive than the documentation for the other language APIs.
We assume you are already familiar with *Connext DDS* and at least
one of the other language APIs. You can check out the
`RTI Community <https://community.rti.com/documentation>`_
for the *Connext DDS* Getting Started Guide, User's Manual
and the reference for the C, C++, Java, and .NET APIs.

RTI also provides *RTI Connector*, a limited, simpler API for Python®
and JavaScript. *RTI Connector* for Python is production-ready
and easier to use, but the *Connext DDS* Python API is more extensive.

This Python API is based on the Modern C++ API and is similar but has a few key
differences.

The main difference is that :class:`DynamicData` is the main way to manipulate
data samples. The type of a DynamicData sample is defined with :class:`DynamicType`.
DynamicTypes can be defined programmatically or loaded from an XML definition.
IDL definitions can be converted to XML.

In addition to DynamicData, you can create *Topics* for the built-in types
:class:`StringTopicType`, :class:`KeyedStringTopicType`, :class:`BytesTopicType`,
and  :class:`KeyedBytesTopicType`.

There are a few other differences that make the API more pythonic.
Most types define properties instead of getters and setters:

.. code-block:: python

    import rti.connextdds as dds
    participant = dds.DomainParticipant(0)
    print(participant.domain_id) # Will print zero


Many classes also provide built-in Python functions that allow iterating,
converting to string, etc. Objects are automatically destroyed when their
reference count reaches zero. Many can also be managed in a :code:`with`
block.

The following example shows how to iterate over the samples in
a :class:`DataReader`.

.. code-block:: python

    with reader.take() as samples:
        for sample in samples:
            print(sample)

The samples are loaned, and the loan is returned when the :code:`with`
block ends. The sample is automatically converted to a string by :code:`print`.
