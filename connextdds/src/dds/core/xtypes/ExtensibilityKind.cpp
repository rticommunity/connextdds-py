#include "PyConnext.hpp"
#include "PySafeEnum.hpp"
#include <dds/core/xtypes/StructType.hpp>

using namespace dds::core::xtypes;

namespace pyrti {

template <>
void process_inits<ExtensibilityKind>(py::module &m, ClassInitList &) {
  init_dds_safe_enum<ExtensibilityKind_def>(
      m, "ExtensibilityKind", [](py::object &o) {
        py::enum_<ExtensibilityKind::type>(o, "Enum")
            .value("FINAL", ExtensibilityKind::type::FINAL,
                   "Final extensibility.")
            .value("EXTENSIBLE", ExtensibilityKind::type::EXTENSIBLE,
                   "Extensible extensibility.")
            .value("MUTABLE", ExtensibilityKind::type::MUTABLE,
                   "Mutable extensibility.")
            .export_values();
      });
}

} // namespace pyrti
