#include "PyConnext.hpp"
#include <rti/core/policy/CorePolicy.hpp>
#include "PySafeEnum.hpp"

using namespace rti::core::policy;

template<>
void pyrti::process_inits<RefilterKind>(py::module& m, pyrti::ClassInitList& l) {
    auto rk = init_dds_safe_enum<rti::core::policy::RefilterKind_def>(m, "RefilterKind");

    py::enum_<RefilterKind::type>(rk, "RefilterKind")
        .value(
            "NOTHING",
            RefilterKind::type::NOTHING,
            "[default] Do not filter existing samples for a new reader."
            "\n\n"
            "On the publishing side, when a new reader matches a writer, the "
            "writer can be configured to filter previously written samples "
            "stored in the writer queue for the new reader. This option "
            "configures the writer to not filter any existing samples for the "
            "reader and the reader will do the filtering."
        )
        .value(
            "EVERYTHING",
            RefilterKind::type::EVERYTHING,
            "Filter all existing samples for a new reader."
            "\n\n"
            "On the publishing side, when a new reader matches a writer, "
            "the writer can be configured to filter previously written "
            "samples stored in the writer queue. This option configures the "
            "writer to filter all existing samples for the reader when a new "
            "reader is matched to the writer."
        )
        .value(
            "ON_DEMAND",
            RefilterKind::type::ON_DEMAND,
            "Filter existing samples only when they are requested by the reader."
            "\n\n"
            "On the publishing side, when a new reader matches a writer, the "
            "writer can be configured to filter previously written samples "
            "stored in the writer queue. This option configures the writer to "
            "filter only existing samples that are requested by the reader."
        )
        .export_values();
}
