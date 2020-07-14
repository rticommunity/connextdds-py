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


# Create a union type with three possible fields
def create_union_type():
    return dds.UnionType(
        "Foo",
        dds.Int32Type(),
        [
            dds.UnionMember("aShort", dds.Int16Type(), 0),
            dds.UnionMember("aLong", dds.Int32Type(), dds.UnionMember.DEFAULT_LABEL),
            dds.UnionMember("aDouble", dds.Float64Type(), 3),
        ],
    )


union_type = create_union_type()

union_data = dds.DynamicData(union_type)

# Set the selected field by writing a value
union_data["aLong"] = 0
info = union_data.member_info(union_data.discriminator_value)
print("The member selected is {}".format(info.name))

# Change the selected field by updating a different one
union_data["aShort"] = 42
info = union_data.member_info(union_data.discriminator_value)
print(
    "The member selected is {} with value {}".format(
        info.name, union_data[union_data.discriminator_value]
    )
)
