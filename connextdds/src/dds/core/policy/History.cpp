#include "PyConnext.hpp"
#include <dds/core/policy/CorePolicy.hpp>
#include "PySafeEnum.hpp"

using namespace dds::core::policy;

namespace pyrti {

template<>
void init_class_defs(py::class_<History>& cls) {
    cls
        .def(
            py::init<>(),
            "Creates a policy that keeps the last sample only."
        )
        .def(
            py::init<
                HistoryKind,
                int32_t
            >(),
            py::arg("kind"),
            py::arg("depth") = 1,
            "Creates a policy with a specific history kind and optionally a "
            "history depth."
        )
        .def_property(
            "kind",
            (HistoryKind (History::*)() const) &History::kind,
            (History& (History::*)(HistoryKind)) &History::kind,
            "The history kind."
        )
        .def_property(
            "depth",
            (int32_t (History::*)() const) &History::depth,
            (History& (History::*)(int32_t)) &History::depth,
            "The history kind."
        )
        .def_property(
            "refilter",
            [](const History& h) {
                return h->refilter();
            },
            [](History& h, rti::core::policy::RefilterKind rk) {
                return h->refilter(rk);
            },
            "The history kind."
        )
        .def_static(
            "keep_all",
            &History::KeepAll,
            "Creates a History with HistoryKind.KEEP_ALL."
        )
        .def_static(
            "keep_last",
            &History::KeepLast,
            py::arg("depth"),
            "Creates a History with HistoryKind.KEEP_LAST and the specified "
            "depth."
        )
        .def(
            py::self == py::self,
            "Test for equality."
        )
        .def(
            py::self != py::self,
            "Test for inequality."
        );
}

static 
void init_dds_history_kind(py::module& m) {
    init_dds_safe_enum<HistoryKind_def>(m, "HistoryKind",
        [](py::object& o) {
            py::enum_<HistoryKind::type>(o, "Enum")
                .value(
                    "KEEP_LAST",
                    HistoryKind::type::KEEP_LAST,
                    "[default] Keep the last depth samples."
                    "\n\n"
                    "On the publishing side, RTI Connext will only attempt to keep "
                    "the most recent depth samples of each instance of data "
                    "(identified by its key) managed by the DataWriter. Invalid "
                    "samples represeting a disposal or unregistration of an instance "
                    "do not count towards the history depth."
                    "\n\n"
                    "On the subscribing side, the DataReader will only attempt to "
                    "keep the most recent depth samples received for each instance "
                    "(identified by its key) until the application takes them via the "
                    "DataReader 's take() operation."
                    "\n\n"
                    "Invalid samples represeting a dispoasal or unregistration of an "
                    "instance do count towards the history depth and may therefore "
                    "replace a value of the instance currently being stored in the "
                    "reader queue."
                )
                .value(
                    "KEEP_ALL",
                    HistoryKind::type::KEEP_ALL,
                    "Keep all the samples."
                    "\n\n"
                    "On the publishing side, RTI Connext will attempt to keep all "
                    "samples (representing each value written) of each instance of "
                    "data (identified by its key) managed by the DataWriter until "
                    "they can be delivered to all subscribers."
                    "\n\n"
                    "On the subscribing side, RTI Connext will attempt to keep all "
                    "samples of each instance of data (identified by its key) managed "
                    "by the DataReader. These samples are kept until the application "
                    "takes them from RTI Connext via the take() operation."
                )
                .export_values();
        }
    );
}

template<>
void process_inits<History>(py::module& m, ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            init_dds_history_kind(m);
            return init_class<History>(m, "History");
        }
    );
}

}
