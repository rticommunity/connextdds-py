.. py:currentmodule:: rti.connextdds

Types
~~~~~

Types in connextdds-py are similar to the types in other RTI APIs
but with a few differences. Due to Python's dynamic nature, :class:`DynamicData`
is used. You cannot directly use IDL files for your types,
however, you can generate XML files from your IDL files. To do so,
at the command line run:

.. code-block:: shell

    $ rtiddsgen -convertToXml your_idl_file.idl 

Generating types this way is very easy. After you have an XML file,
you can get the type from the file like this:

.. code-block:: python

    import rti.connextdds as dds
    FILE = "path to your xml file"
    provider = dds.QosProvider(FILE)
    provider_type = provider.type("NameOfType")

Now in the :code:`provider_type` variable you have the type defined in the IDL file.
To make an instance of it, all you have to do is this:

.. code-block:: python
    
    sample = dds.DynamicData(provider_type)
    # Let's say we have a field x that takes a 32 bit integer,
    # we can assign it like this
    sample['x'] = 42
    
Now you would be able to publish the sample which is discussed in
:ref:`writer:Publications`.

Sequences
=========

Sequences allow you to group data together and randomly access them.
The easiest way to use a sequence is to define it in an IDL file then
convert it to XML. Once it is XML, you can use the type. With sequences
you have two options for accessing and two options for modifying.
You can access it element by element or by getting the whole sequence at
once. you can set it element by element or assigning a new sequence to it.


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

    # Assuming we already have a datawriter for the correct type
    writer.write({"x": 1, "y": 42})
    # This will work if the data type that writer is working with
    # is a structure with values x and y that are of type integer

Nested Sample Data
==================

Nested data allows you to have some complex data structures.

.. code-block:: python

    # Here we are creating a nested type
    nested_type = dds.StructType("nested_type")
    # Here we use COORDINATE_TYPE, a type that would be
    # defined in XML as two integers, an x and a y value
    nested_type.add_member(dds.Member("a", COORDINATE_TYPE))
    sample = dds.DynamicData(nested_type)
    sample["a.x"] = 1
    sample["a.y"] = 2
    # Now we have values that are nested in a struct

Loaned Data
===========

Loans allow you to safely read and write data without race conditions.
An example of this can be seen in the :ref:`types:Sequences` section of this document.
