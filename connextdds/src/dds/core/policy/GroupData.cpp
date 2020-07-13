#include "PyConnext.hpp"
#include <dds/core/policy/CorePolicy.hpp>

using namespace dds::core::policy;

namespace pyrti {

template<>
void init_class_defs(py::class_<GroupData>& cls) {
    cls
        .def(
            py::init<>(),
            "Create a GroupData instance."
        )
        .def(
            py::init<
                const std::vector<uint8_t>&
            >(),
            py::arg("bytes"),
            "Create a GroupData instance with a sequence of bytes."
        )
        .def_property(
            "value",
            (const std::vector<uint8_t> (GroupData::*)() const) &GroupData::value,
            (std::vector<uint8_t>& (GroupData::*)(std::vector<uint8_t>&) const) &GroupData::value,
            "The byte sequence of this GroupData."
        )
        .def(
            "__iter__",
            [](const GroupData& gd) {
                return py::make_iterator(gd.value().begin(), gd.value().end());
            },
            py::keep_alive<0, 1>()
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

template<>
void process_inits<GroupData>(py::module& m, ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return init_class<GroupData>(m, "GroupData");
        }
    );
}

}
