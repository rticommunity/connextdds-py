.. py:currentmodule:: rti.connextdds

Data Types
~~~~~~~~~~

In data-centric communications, the applications participating in
the communication need to share a common view of the types of data
being passed around.

The *Connext DDS* Python API loads type definitions from XML. Other
*Connext DDS* APIs support static types generated from an IDL definition.
You can generate XML files from those IDL files using ``rtiddsgen``:

.. code-block:: shell

    $ rtiddsgen -convertToXml your_types.idl

To load a type from the application:

.. code-block:: python

    import rti.connextdds as dds
    provider = dds.QosProvider("your_types.xml")
    my_type = provider.type("MyType")

You can now use ``my_type`` to create a ``Topic`` (see :ref:`topic:Topics`)
and to instantiate a :class:`DynamicData` object:

.. code-block:: python

    sample = dds.DynamicData(my_type)

    # Let's say my_type has an int32 field
    sample["x"] = 42

Now you would be able to publish the sample, which is discussed in
:ref:`writer:Publications`.

Defining Types Programmatically
===============================

Types can also be defined dynamically in the application, using :class:`DynamicType`
and its derived classes.

The following example creates a type and instantiates a data sample:

.. code-block:: python

    # struct Point {
    #     double x, y;
    # };
    point_type = dds.StructType("Point")
    point_type.add_member(dds.Member("x", dds.Float64Type()))
    point_type.add_member(dds.Member("y", dds.Float64Type()))

    # struct MyType {
    #     @key string<128> id;
    #     Point location;
    #     int32 int_array[5];
    #     sequence<Point, 10> path;
    # };
    my_type = dds.StructType("MyType")
    my_type.add_member(dds.Member(name="id", data_type=dds.StringType(128), is_key=True))
    my_type.add_member(dds.Member(name="location", data_type=point_type))
    my_type.add_member(dds.Member(name="int_array", data_type=dds.ArrayType(dds.Int32Type(), 5)))
    my_type.add_member(dds.Member(name="path", data_type=dds.SequenceType(point_type, 10)))

    # Instantiate the type
    sample = dds.DynamicData(my_type)
    sample["id"] = "object1"

Accessing Nested Members
========================

There are a few different ways to manipulate data with nested
types. The ``.`` notation allows accessing nested primitive members at any level:

.. code-block:: python

    sample = dds.DynamicData(my_type)
    sample["location.x"] = 1.5
    sample["location.y"] = 2.5

To make multiple modifications to a complex member, you can get a temporary
reference (a loan) to the member:

.. code-block:: python

    with sample.loan_value("location") as location:
        location.data["x"] = 11.5
        location.data["y"] = 12.5

A nested member can be assigned from a dictionary, too:

.. code-block:: python

    sample["location"] = {"x": 4.5, "y": 5.5}
    print(sample["location"])

Accessing Sequences and Arrays
==============================

Sequences and arrays can be retrieved or set using Python lists:

.. code-block:: python

    # We're using the type we created before
    sample = dds.DynamicData(my_type)

    # Set the array field with the values of a python list
    sample["int_array"] = [1, 2, 3, 4, 5]

    # Get all the array elements in a python list
    lst = list(sample["int_array"])

    # Set and get a single element:
    sample["int_array[1]"] = 4
    value = sample["int_array[1]"]

Lists of structures can be accessed using lists of dictionaries:

.. code-block:: python

    sample["path"] = [{"x": 1, "y": 2}, {"x": 3, "y": 4}]
    path = list(sample["path"])

If you only need to set a few elements or fields, you can loan the sequence
and its elements. Sequences are automatically resized when you
access and index above the current length:

.. code-block:: python

    with sample.loan_value("path") as path:
        with path.data.loan_value(2) as point:
            point.data["x"] = 111
            point.data["y"] = 222
    print(sample["path[2].x"]) # prints 111


