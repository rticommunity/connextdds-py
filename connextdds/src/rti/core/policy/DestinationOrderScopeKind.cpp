#include "PyConnext.hpp"
#include "PySafeEnum.hpp"
#include <rti/core/policy/CorePolicy.hpp>

using namespace rti::core::policy;

namespace pyrti {

template <>
void process_inits<DestinationOrderScopeKind>(py::module &m, ClassInitList &l) {
  init_dds_safe_enum<DestinationOrderScopeKind_def>(
      m, "DestinationOrderScopeKind", [](py::object &o) {
        py::enum_<DestinationOrderScopeKind::type>(o, "Enum")
            .value(
                "INSTANCE", DestinationOrderScopeKind::type::INSTANCE,
                "[default] Indicates that data is ordered on a per instance "
                "basis "
                "if used along with DestinationOrderKind.BY_SOURCE_TIMESTAMP. "
                "\n\n"
                "The source timestamp of the current sample is compared to the "
                "source timestamp of the previously received sample for the "
                "same "
                "instance. The tolerance check is also applied per instance.")
            .value(
                "TOPIC", DestinationOrderScopeKind::type::TOPIC,
                "Indicates that data is ordered on a per topic basis if used "
                "along with DestinationOrderKind.BY_SOURCE_TIMESTAMP."
                "\n\n"
                "The source timestamp of the current sample is compared to the "
                "source timestamp of the previously received sample for the "
                "same "
                "topic. The tolerance check is also applied per topic.")
            .export_values();
      });
}

} // namespace pyrti
