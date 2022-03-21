/*
 * (c) 2020 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 *
 * RTI grants Licensee a license to use, modify, compile, and create derivative
 * works of the Software solely for use with RTI products.  The Software is
 * provided "as is", with no warranty of any type, including any warranty for
 * fitness for any purpose. RTI is under no obligation to maintain or support
 * the Software.  RTI shall not be liable for any incidental or consequential
 * damages arising out of the use or inability to use the software.
 */

#include "PyConnext.hpp"
#include <dds/dds.hpp>

using namespace dds::core::xtypes;

void init_namespace_dds_core_xtypes(py::module& m, pyrti::ClassInitList& l, pyrti::DefInitVector&)
{
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
