#include "PyConnext.hpp"
#include <rti/core/status/Status.hpp>

using namespace rti::core::status;

namespace pyrti {

template<>
void init_class_defs(py::class_<ReliableReaderActivityChangedStatus>& cls) {
    cls
        .def_property_readonly(
            "active_count",
            &ReliableReaderActivityChangedStatus::active_count,
            "The current number of reliable DataReaders currently matched "
            "with this reliable DataWriter."
        )
        .def_property_readonly(
            "inactive_count",
            &ReliableReaderActivityChangedStatus::inactive_count,
            "The number of reliable DataReaders that have been dropped by "
            "this reliable DataWriter because they failed to send "
            "acknowledgments in a timely fashion."
        )
        .def_property_readonly(
            "last_instance_handle",
            &ReliableReaderActivityChangedStatus::last_instance_handle,
            "The instance handle of the last reliable remote DataReader to be "
            "determined inactive."
        );
}

template<>
void process_inits<ReliableReaderActivityChangedStatus>(py::module& m, ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return init_class<ReliableReaderActivityChangedStatus>(m, "ReliableReaderActivityChangedStatus");
        }
    );
}

}
