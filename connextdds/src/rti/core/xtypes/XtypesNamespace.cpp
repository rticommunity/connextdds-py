#include "PyConnext.hpp"
#include <rti/rti.hpp>

using namespace rti::core::xtypes;
using namespace dds::core::xtypes;

void init_namespace_rti_core_xtypes(py::module &m, pyrti::ClassInitList &l) {
  pyrti::process_inits<AbstractConstructedType<EnumMember>>(m, l);
  pyrti::process_inits<AbstractConstructedType<Member>>(m, l);
  pyrti::process_inits<AbstractConstructedType<UnionMember>>(m, l);
  pyrti::process_inits<DynamicDataInfo>(m, l);
  pyrti::process_inits<DynamicDataMemberInfo>(m, l);
  pyrti::process_inits<DynamicDataProperty>(m, l);
  pyrti::process_inits<DynamicDataTypeSerializationProperty>(m, l);
  pyrti::process_inits<LoanedDynamicData>(m, l);
  pyrti::process_inits<UnidimensionalCollectionTypeImpl>(m, l);
}