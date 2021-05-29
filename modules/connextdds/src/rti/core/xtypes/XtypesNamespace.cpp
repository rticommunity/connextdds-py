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
#include <rti/rti.hpp>

using namespace rti::core::xtypes;
using namespace dds::core::xtypes;

void init_namespace_rti_core_xtypes(py::module& m, pyrti::ClassInitList& l)
{
    pyrti::process_inits<AbstractConstructedType<EnumMember>>(m, l);
    pyrti::process_inits<AbstractConstructedType<Member>>(m, l);
    pyrti::process_inits<AbstractConstructedType<UnionMember>>(m, l);
    pyrti::process_inits<DynamicDataInfo>(m, l);
    pyrti::process_inits<DynamicDataMemberInfo>(m, l);
    pyrti::process_inits<DynamicDataProperty>(m, l);
    pyrti::process_inits<DynamicDataTypeSerializationProperty>(m, l);
    pyrti::process_inits<DynamicTypePrintFormatProperty>(m, l);
    pyrti::process_inits<LoanedDynamicData>(m, l);
    pyrti::process_inits<UnidimensionalCollectionTypeImpl>(m, l);
}