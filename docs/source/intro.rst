.. py:currentmodule:: rti.connextdds

Introduction
~~~~~~~~~~~~

Welcome to connextdds-py! This is the Python API for RTI's ConnextDDS. 
It is based on the Modern C++ API and is similar but has a few key 
differences. Due to the dynamic nature of Python, Dynamic Data is 
the way to define data types. This can be done in IDL files that
can be converted to XML files, or in code.

The API is almost identical to the Modern C++ API but we follow 
Python conventions. For example, instead of getters and setters, we 
utilize properties. If there's a property that is write only,
it is assigned typically with a call to set_name_of_property.

**Note: It is highly recommmended that you have some experience with
at least one full ConnextDDS version**

It is recommended that you read the following documentation to 
understand the Python library.

- :ref:`participant:Participant`
- :ref:`types:Types`
- :ref:`topic:Topic`
- :ref:`writer:Writer`
- :ref:`reader:Reader`
