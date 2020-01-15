#include "PyConnext.hpp"
#include <dds/dds.hpp>

using namespace dds::core::xtypes;

void init_namespace_dds_core_xtypes(py::module& m, pyrti::ClassInitList& l) {
    pyrti::process_inits<AliasType>(m, l);
    pyrti::process_inits<ArrayType>(m, l);
    pyrti::process_inits<CollectionType>(m, l);
    pyrti::process_inits<DynamicData>(m, l);
    pyrti::process_inits<DynamicType>(m, l);
    pyrti::process_inits<EnumMember>(m, l);
    pyrti::process_inits<EnumType>(m, l);
    pyrti::process_inits<ExtensibilityKind>(m, l);
    pyrti::process_inits<Member>(m, l);
    pyrti::process_inits<pyrti::PyPrimitiveType>(m, l);
    pyrti::process_inits<SequenceType>(m, l);
    pyrti::process_inits<StringType>(m, l);
    pyrti::process_inits<StructType>(m, l);
    pyrti::process_inits<TypeKind>(m, l);
    pyrti::process_inits<UnionMember>(m, l);
    pyrti::process_inits<UnionType>(m, l);
    pyrti::process_inits<WStringType>(m, l);
}
