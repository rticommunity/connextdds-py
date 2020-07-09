"""
 (c) 2020 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 RTI grants Licensee a license to use, modify, compile, and create derivative
 works of the Software.  Licensee has the right to distribute object form only
 for use with RTI products.  The Software is provided "as is", with no warranty
 of any type, including any warranty for fitness for any purpose. RTI is under
 no obligation to maintain or support the Software.  RTI shall not be liable for
 any incidental or consequential damages arising out of the use or inability to
 use the software.
 """

import rti.connextdds as dds
import textwrap


# Create an inner struct with x and y coordinates
def create_typecode_inner_struct():
    return dds.StructType(
        "InnerStruct",
        [dds.Member("x", dds.Float64Type()), dds.Member("y", dds.Float64Type())],
    )


# Create an container struct for the inner struct that was defined
def create_typecode_outer_struct():
    outer_struct = dds.StructType("OuterStruct")
    outer_struct.add_member(dds.Member("inner", create_typecode_inner_struct()))
    return outer_struct


inner_type = create_typecode_inner_struct()
outer_type = create_typecode_outer_struct()

outer_data = dds.DynamicData(outer_type)
inner_data = dds.DynamicData(inner_type)

print(
    textwrap.dedent(
        """\
        Connext DynamicData Nested Struct Example 
        -----------------------------------------
        Data Type
        ---------"""
    )
)

inner_type.print_idl()
print("")
outer_type.print_idl()
print("")

inner_data["x"] = 3.14159
inner_data["y"] = 2.71828

print(
    textwrap.dedent(
        """\
        Setting the initial values of struct 
        ------------------------------------"""
    )
)

outer_data["inner"] = inner_data
print(outer_data)

inner_data.clear_all_members()
inner_data = outer_data["inner"]

print(
    textwrap.dedent(
        """
         + copied struct from outer_data
         + inner_data value"""
    )
)

print(inner_data)

print("\n+ setting new values to inner_data")
inner_data["x"] = 1.00000
inner_data["y"] = 0.00001

print(inner_data)
print("+ current outer_data value")
print("{}\n".format(outer_data))

print(
    textwrap.dedent(
        """\n
         loan/unloan API
         ---------------
         + loan member called"""
    )
)

with outer_data.loan_value("inner") as loaned_inner:
    print(loaned_inner.data)
    print("+ setting new values to loaned_data")

    loaned_inner.data.set_float64("x", 1.00000)
    loaned_inner.data["y"] = 0.00001

    print(loaned_inner.data)

print("+ current outer_data value")
print(outer_data)
