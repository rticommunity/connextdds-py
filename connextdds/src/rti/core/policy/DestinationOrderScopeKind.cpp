#include "PyConnext.hpp"
#include <rti/core/policy/CorePolicy.hpp>
#include "PySafeEnum.hpp"

using namespace rti::core::policy;

template<>
void pyrti::process_inits<DestinationOrderScopeKind>(py::module& m, pyrti::ClassInitList& l) {
    auto dosk = init_dds_safe_enum<DestinationOrderScopeKind_def>(m, "DestinationOrderScopeKind");

    py::enum_<DestinationOrderScopeKind::type>(dosk, "DestinationOrderScopeKind")
        .value(
            "INSTANCE",
            DestinationOrderScopeKind::type::INSTANCE,
            "[default] Indicates that data is ordered on a per instance basis "
            "if used along with DestinationOrderKind.BY_SOURCE_TIMESTAMP. "
            "\n\n"
            "The source timestamp of the current sample is compared to the "
            "source timestamp of the previously received sample for the same "
            "instance. The tolerance check is also applied per instance."
        )
        .value(
            "TOPIC",
            DestinationOrderScopeKind::type::TOPIC,
            "Indicates that data is ordered on a per topic basis if used "
            "along with DestinationOrderKind.BY_SOURCE_TIMESTAMP."
            "\n\n"
            "The source timestamp of the current sample is compared to the "
            "source timestamp of the previously received sample for the same "
            "topic. The tolerance check is also applied per topic."
        )
        .export_values();
}
