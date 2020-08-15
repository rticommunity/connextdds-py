.. py:currentmodule:: rti.connextdds

Types
~~~~~

In data-centric communications, the applications participating in
the communication need to share a common view of the types of data
being passed around.

The Connext DDS Python API loads type definitions from XML. Other
Connext DDS APIs support static types generated from an IDL definition.
You can generate XML files from those IDL files. To do so,
at the command line run:

.. code-block:: shell

    $ rtiddsgen -convertToXml your_idl_file.idl 

To load a type from the application:

.. code-block:: python

    import rti.connextdds as dds
    FILE = "path to your xml file"
    provider = dds.QosProvider(FILE)
    my_type = provider.type("NameOfType")

You can now use `my_type` to create a Topic (see :ref:`topic:Topics`
or to instantiate a DynamicData object:

.. code-block:: python
    
    sample = dds.DynamicData(my_type)

    # Let's say we have a field x that takes a 32 bit integer,
    # we can assign it like this
    sample['x'] = 42
    
Now you would be able to publish the sample which is discussed in
:ref:`writer:Publications`.

Defining types programmatically
===============================

Types can also be defined in the application, using :class:`DynamicType` and
its derived classes.

.. code-block:: python

    # Here we are creating a nested type
    nested_type = dds.StructType("nested_type")
    # Here we use COORDINATE_TYPE, a type that would be
    # defined in XML as two integers, an x and a y value
    nested_type.add_member(dds.Member("a", COORDINATE_TYPE))

Nested types
============

There are a few different ways to manipulate data with nested
types. The `.` notation allows accessing nested primitive members:

    sample = dds.DynamicData(nested_type)
    sample["a.x"] = 1
    sample["a.y"] = 2

Nested members can be loaned:


Sequences and arrays
====================

Sequences and arrays can be retrieved or set as a full python list,
or element by element:


.. code-block:: python

    # my_type will have been set as discussed above
    # my_type will have a sequence called MySeq that is an integer
    # type of size 100
    sample = dds.DynamicData(my_type)
    
    # Options for setting
    sample["MySeq"] = [42] * 100
    
    # OR you can do this
    loan = sample.loan_value("MySeq")
    data = loan.data
    for i in range(0, 100):
        data[i] = 42
    # note this version is less efficient 
    loan.return_loan()

    # Options for getting
    lst = sample["MySeq"]


It is also possible to loan a sequence or array (or an element if the type is complex):

.. code-block:: python

    # OR you can do this
    loan = sample.loan_value("MySeq")
    data = loan.data
    print(data[42])
    loan.return_loan()


Data from Dictionaries
======================

You can also use native python dictionaries with DynamicData.
For example,

.. code-block:: python

    # Assuming we already have a DataWriter for the correct type
    writer.write({"x": 1, "y": 42})
    # This will work if the data type that writer is working with
    # is a structure with values x and y that are of type integer


