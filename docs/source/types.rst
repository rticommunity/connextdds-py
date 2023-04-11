.. py:currentmodule:: rti.connextdds

Data Types
~~~~~~~~~~

IDL to Python mapping
---------------------

If you're not familiar with IDL and DDS data types, you can read and complete the
exercises in the `RTI Connext Getting Started Guide <https://community.rti.com/static/documentation/connext-dds/7.1.0/doc/manuals/connext_dds_professional/getting_started_guide/index.html>`_,
chapter "3. Data Types".

You can generate your Python data types from IDL (or XML) using **rtiddsgen**:

.. code-block:: bash

    rtiddsgen -language python MyTypes.idl [-example universal]

(The optional ``-example universal`` option generates example publisher and
subscriber scripts.)

You can also define your types directly in Python, without code generation,
following a few conventions.

IDL structs map to Python dataclasses with a special decorator. For example, the
following IDL struct:

.. code-block:: idl

    struct Point {
        int64 x;
        int64 y;
    };

Is defined in Python as follows:

.. code-block:: python

    import rti.types as idl

    @idl.struct
    class Point:
        x: int = 0
        y: int = 0

The ``@idl.struct`` decorator parses the type and generates the necessary type
support routines that allow DDS to serialize and deserialize instances of this
class. ``@idl.struct`` also applies ``@dataclass`` to the class, adding all
its functionality (``__init__``, ``__eq__``, ``__repr__``, ``__hash__``,
``copy.deepcopy()`` support, etc.). For example:

.. code-block:: python

    point1 = Point(x=1, y=2)
    point2 = Point(y=2) # x=0, y=2
    point3 = Point() # x=0, y=0

    print(point1)
    assert point1 != point2

    point2.x = 1
    assert point1 == point2

    point4 = copy.deepcopy(point3)
    assert point4 == point3

``Point`` can be used to create a :class:`Topic` as we explained
in :ref:`topic:Topics`.

There are a few pre-defined classes available for convenience
(see :ref:`types:Built-in types` below).

The following sections explain how different IDL types map to Python.

Primitive types
===============
The IDL types ``int64``, ``double``, and ``bool`` map directly to the Python
built-in types ``int``, ``float``, and ``bool``.

For IDL types of different sign or size, type annotations are provided. For 
example, the IDL types ``int32``, ``uint16``, and ``float`` map to the type 
annotations ``idl.int32``, ``idl.uint16``, and ``idl.float32``, respectively.

For example, the following IDL type:

.. code-block:: idl

    struct Foo {
        int32 a;
        int64 b;
        uint64 c;
        float d;
        double e;
        bool f;
    };

Maps to the following Python class:

.. code-block:: python

    @idl.struct
    class Foo:
        a: idl.int32
        b: int
        c: idl.uint64
        d: idl.float32
        e: float
        f: bool

(Note that IDL's ``double`` maps to Python's ``float``, and IDL's ``float`` to
``idl.float32``.)

.. warning::

    The sign or size of the types is currently not enforced. If you write a
    value outside the expected range, the subscribers will receive an incorrect
    value.


Strings
=======

IDL strings map to Python's built-in ``str``.

IDL strings can be single (UTF-8) or wide (UTF-16) and bounded or unbounded.
These options are passed to the ``member_annotations`` argument of the
type decorator, if needed. By default strings are UTF-8 and unbounded.

For example, the following IDL type:

.. code-block:: idl

    // MyTypes.idl
    struct MyStrings {
        string unbounded_str;
        string<128> bounded_str;
        wstring<256> bounded_wstr;
    };

Maps to the following Python dataclass:

.. code-block:: python

    @idl.struct(
        member_annotations = {
            'bounded_str': [idl.bound(128)],
            'bounded_wstr': [idl.bound(256), idl.utf16],
        }
    )
    class MyStrings:
        unbounded_str: str = ""
        bounded_str: str = ""
        bounded_wstr: str = ""

The type above can be generated with **rtiddsgen** as follows:

.. code-block:: bash

    rtiddsgen -unboundedSupport -language python MyTypes.idl


Sequences
=========

The mapping of IDL sequences depends on whether the element type is a primitive
type or not.

Non-primitive sequences map to Python's ``list``.

Primitive sequences map, by default, to efficient, compact collections in the
``dds`` module. For example, an IDL ``sequence<int32>`` maps to ``dds.Int32Seq``.

IDL sequences can be bounded or unbounded. Bounded sequences may not exceed the
number of elements indicated by the bound when the data is written. The bound
is specified as part of the ``member_annotations`` argument to the type
decorator.

For example, the following IDL type:

.. code-block:: idl

    struct MySequences {
        sequence<int64, 100> bounded_int64_seq;
        sequence<uint32> unbounded_uint32_seq;
        sequence<Foo> unbounded_foo_seq;
    };

Maps to the following Python type:

.. code-block:: python

    @idl.struct(
        member_annotations = {
            'bounded_int64_seq': [idl.bound(100)],
        }
    )
    class MySequences:
        bounded_int64_seq: Sequence[int] = field(default_factory = idl.array_factory(int))
        unbounded_uint32_seq: Sequence[idl.uint32] = field(default_factory = idl.array_factory(idl.uint32))
        unbounded_foo_seq: Sequence[Foo] = field(default_factory = list)

The ``field`` function and its ``default_factory`` argument indicate how the
``dataclass`` is created by default. When a instance of ``MySequences``
is created, all the sequences are empty. You can add elements or replace them
altogether. For example:

.. code-block:: python

    my_sequences = MySequences()
    my_sequences.bounded_int64_seq.append(1)
    my_sequences.bounded_int64_seq.append(2)
    my_sequences.unbounded_uint32_seq = dds.Uint32Seq([33] * 5)
    my_sequences.unbounded_foo_seq = [Foo(a=x) for x in range(10)]

You're not restricted to ``dds.Int64Seq`` or ``dds.Uint32Seq``; you can
write a ``list``, but the data serialization will be less efficient.

Arrays
======

The mapping for IDL arrays is similar to that of sequences, except that an array
must always have the same number of elements.

The default creation of data samples with arrays populates them with the right
number of elements.

The ``write()`` operation will fail if a sample with an array containing an
incorrect number of elements is written.

.. warning ::

    Multi-dimensional arrays are not fully supported in this release. They are
    flattened out and the number of elements is the product of the array's
    dimensions.

Nested collections
==================

In IDL, you can define sequences of sequences, sequences of arrays, and
arrays of sequences.

To do that, the inner collection must be aliased. For example:

.. code-block:: idl

    typedef sequence<Point> PointSeq;
    typedef int64 TenInts[10];

    struct MySequences {
        sequence<PointSeq> sequence_of_point_sequences;
        sequence<TenInts> sequence_of_int_arrays;
        PointSeq five_point_sequences[5];
    };


Optional members
================

By default, members of an IDL ``struct`` always contain a value and they are
always published with a data sample. A member can be declared optional in IDL
allowing it to not be sent with all data samples.

In Python, optional members receive the ``None`` value by default.

For example, the following IDL struct contains a required and an optional member:

.. code-block:: idl

    struct MyOptionals {
        double required_value;
        @optional double optional_value;
    };

This maps to the following Python dataclass:

.. code-block:: python

    @idl.struct
    class MyOptionals:
        required_value: float = 0.0
        optional_value: Optional[float] = None

And a data sample is created by default as follows:

.. code-block:: python

    sample = MyOptionals()
    assert sample.required_value == 0.0
    assert sample.optional_value is None

Enumerations
============

IDL enumerations map to Python ``IntEnum``-derived classes that are decorated
with the ``idl.enum`` decorator.

.. code-block:: idl

    enum Color {
        RED,
        GREEN,
        BLUE
    };

Maps to:

.. code-block:: python

    @idl.enum
    class Color(IntEnum):
        RED = 0
        GREEN = 1
        BLUE = 2

Unions
======

IDL unions define types in which only one member exists at a time. The selected
member is identified by the "discriminator."

IDL unions map to decorated Python dataclasses with two members (``discriminator`` and ``value``)
and one read/write property for each member that allows setting the value and
the discriminator consistently.

For example, the following IDL union:

.. code-block:: idl

    union MyUnion switch(int32) {
        case 0:
            string string_member;
        case 1:
            int64 int_member;
        case 2:
            Point point_member;
    };

Maps to the following Python class:

.. code:: python

    @idl.union
    class MyUnion:

        discriminator: idl.int32 = 0
        value: Union[str, int, Point] = ""

        string_member: str = idl.case(0)
        int_member: int = idl.case(1)
        point_member: Point = idl.case(2)

The ``discriminator`` and ``value`` members should be used as read-only. To
modify the ``union``, use the "cases" (read/write properties). For example:

.. code:: python

    sample = MyUnion()

    # By default the case with the lowest discriminator value (0 in this case)
    # is selected (unless a "default:" label is defined in IDL)
    assert sample.discriminator == 0
    assert sample.value == ""
    assert sample.string_member == ""

    # Select a different member:
    sample.point_member = Point(1, 2)
    assert sample.discriminator == 2
    assert sample.value == Point(1, 2)
    assert sample.point_member == Point(1, 2)

    # Attempting to access member that is not selected raises a ValueError:
    try:
        print(sample.string_member)
    except ValueError:
        print("string_member is not selected")

Modules
=======

Each IDL (or XML) file called **Foo.idl** generates a Python file with the same
name, **Foo.py**.

This defines a python package you can import:

.. code:: python

    import Foo

    my_type = Foo.MyType()

Additionally, in IDL you can define "modules." Similarly to C++ namespaces,
an IDL module can be partially defined in several files. To allow for this
capability, IDL modules map to Python's `SimpleNamespace <https://docs.python.org/3/library/types.html#types.SimpleNamespace>`_.

For example, assume the following IDL files:

.. code:: idl

    # Foo.idl

    module A {
        struct MyType1 { ... };
    };

    struct MyType2 { ... };

And:

.. code:: idl

    # Bar.idl

    module A {
        struct MyType3 { ... };
    };

    module B {
        struct MyType4 { ... };
    };

This generates two Python packages, **Foo.py** and **Bar.py**. The module **A**
is accessible from both packages as ``Foo.A`` and ``Bar.A``. **Foo.idl** also
defines a type without a module, and **Bar.idl** defines another module, **B**:


.. code:: python

    import Foo
    import Bar

    sample1 = Foo.A.MyType1()
    sample2 = Foo.MyType2()
    sample3 = Bar.A.MyType3()
    sample4 = Bar.B.MyType4()

    # You can create an alias:
    MyType3 = Bar.A.MyType3

    sample3 = MyType3()

IDL annotations
===============

There are several IDL annotations that are passed to the ``struct``, ``union``,
or ``enum`` decorators in the ``type_annotations`` or ``member_annotations``
arguments.

Examples are the ``@key`` and extensibility annotations (such as ``@mutable``):

.. code-block:: idl

    @mutable
    struct MutableKeyedType {
        @key string id;
        string value;
    };

The Python mapping is:

.. code:: python

    @idl.struct(
        type_annotations = [idl.mutable],
        member_annotations = {
            'id': [idl.key]
        }
    )
    class MutableKeyedType:
        id: str = ""
        value: str = ""

These annotations don't have a direct effect on how you use the classes in your
application, but they may change how the data is internally processed or
delivered.


Built-in types
--------------

For convenience, the following types are directly available in the
``rti.types.builtin`` package:

.. code:: python

    @idl.struct
    class String:
        value: str = ""


    @idl.struct(member_annotations={'key': [idl.key]})
    class KeyedString:
        key: str = ""
        value: str = ""


    @idl.struct
    class Bytes:
        value: Sequence[idl.uint8] = field(default_factory=idl.array_factory(idl.uint8))


    @idl.struct(member_annotations={'key': [idl.key]})
    class KeyedBytes:
        key: str = ""
        value: Sequence[idl.uint8] = field(default_factory=idl.array_factory(idl.uint8))

You can directly use these types in your application:

.. code:: python

    import rti.connextdds as dds
    from rti.types.builtin import String

    participant = dds.DomainParticipant(domain_id=0)
    topic = dds.Topic(participant, "HelloWorld", String)
    writer = dds.DataWriter(participant, topic)
    writer.write(String("Hello World!"))


Type support
------------

Every ``@idl.struct``-decorated class or ``@idl.union``-decorated class has an
associated ``TypeSupport`` object that can be obtained as follows:

.. code:: python

    import rti.types as idl

    @idl.struct
    class Foo:
        ...

    foo_support = idl.get_type_support(Foo)


``TypeSupport`` provides access to serialization functions:

.. code:: python

    foo = Foo()
    buffer = foo_support.serialize(foo)
    new_foo = foo_support.deserialize(buffer)
    assert foo == new_foo

It also provides the property ``max_serialized_sample_size``,
and the method ``get_serialized_sample_size()``.

``TypeSupport`` also provides information about the type definition as a
:class:`DynamicType` (``dynamic_type`` property) and helpers to convert to and from
:class:`DynamicData` (``to_dynamic_data()`` and ``from_dynamic_data()`` methods).


DynamicType and DynamicData
---------------------------

The *Connext* Python API can dynamically load type definitions from XML and
create ``dds.DynamicData`` samples.

.. code-block:: python

    import rti.connextdds as dds

    provider = dds.QosProvider("your_types.xml")
    my_type = provider.type("MyType")

You can now use ``my_type`` to create a :class:`DynamicData.Topic`
and to instantiate a :class:`DynamicData` object:

.. code-block:: python

    topic = dds.DynamicData.Topic(participant, "Example MyType", my_type)
    sample = dds.DynamicData(my_type)
    sample["x"] = 42 # assuming MyType has an int32 field

You can use ``topic`` to create a :class:`DynamicData.DataWriter` or a
:class:`DynamicData.DataReader`.

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


