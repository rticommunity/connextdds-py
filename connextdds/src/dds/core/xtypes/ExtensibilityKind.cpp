#include "PyConnext.hpp"
#include <dds/core/xtypes/StructType.hpp>
#include "PySafeEnum.hpp"

using namespace dds::core::xtypes;

namespace pyrti {

template<>
void process_inits<ExtensibilityKind>(py::module& m, ClassInitList&) {
    auto ek = init_dds_safe_enum<ExtensibilityKind_def>(m, "ExtensibilityKind");

    py::enum_<ExtensibilityKind::type>(ek, "ExtensibilityKind")
        .value(
            "FINAL",
            ExtensibilityKind::type::FINAL,
            "Final extensibility."
        )
        .value(
            "EXTENSIBLE",
            ExtensibilityKind::type::EXTENSIBLE,
            "Extensible extensibility."
        )
        .value(
            "MUTABLE",
            ExtensibilityKind::type::MUTABLE,
            "Mutable extensibility."
        )
        .export_values();
}

}
