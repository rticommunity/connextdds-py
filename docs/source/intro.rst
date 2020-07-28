.. py:currentmodule:: rti.connextdds

Introduction
~~~~~~~~~~~~

Welcome to connextdds-py! This is the Python API for RTI Connext DDS. 
It is based on the Modern C++ API and is similar but has a few key 
differences. Due to the dynamic nature of Python, :class:`DynamicData` is 
the way to define data types. This can be done in IDL files that
can be converted to XML files, or in code. Additionally, there are 
a few built in types that can be created in code such as :class:`Int32Type` 
or :class:`StringType`. There are some other differences too. 
For example, instead of getters and setters, we utilize properties.
If there's a property that is write only, it is assigned typically 
with a call to set_name_of_property. 

.. code-block:: python

    import rti.connextdds as dds 
    DOMAIN_ID = 0 # Could be anything, I just chose 0
    participant = dds.DomainParticipant(DOMAIN_ID)
    print(participant.domain_id) # Will print zero

There is also support for built in python constructs to make the
library more ergonomic. For example, instead of a :code:`to_string` 
function anywhere, the python :code:`str()` is used. This allows you
to do things like print a value without it being of type string.
Another built in construct that is used is the :code:`iter()` function.
This function alls the user to iterate through a data type. You can
also use :code:`for a in b` loop constructs. An example would look like 
this:

.. code-block:: python

   # Assume that the reader has been properly created
   samples = reader.take()
   for sample in samples:
       print(sample)

**Note**: As an experimental product, this API documentation is less 
comprehensive than the documentation for the other language APIs.
We assume you are already familiar with Connext DDS and at least
one of the other language APIs. You can check out the 
`RTI Community <https://community.rti.com/documentation>`_
for the Connext DDS Getting Started Guide, User's Manual 
and the reference for the C, C++, Java, and .NET APIs.

RTI also provides RTI Connector, a limited, simpler API for Python 
and JavaScript. RTI Connector for Python is production-ready, but 
the RTI Connext DDS Python API is more complete.

It is recommended that you read the following documentation to 
understand the Python library.
