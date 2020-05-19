import rti.connextdds as dds


def create_union_type():
    return dds.UnionType(
        'Foo',
        dds.Int32Type(),
        [
            dds.UnionMember('aShort', dds.Int16Type(), 0),
            dds.UnionMember('aLong', dds.Int32Type(), dds.UnionMember.DEFAULT_LABEL),
            dds.UnionMember('aDouble', dds.Float64Type(), 3)
        ]
    )


union_type = create_union_type()

union_data = dds.DynamicData(union_type)

union_data['aLong'] = 0
info = union_data.member_info(union_data.discriminator_value)
print('The member selected is {}'.format(info.name))

union_data['aShort'] = 42
info = union_data.member_info(union_data.discriminator_value)
print('The member selected is {} with value {}'.format(
    info.name, union_data[union_data.discriminator_value]))