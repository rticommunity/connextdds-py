#include "PyConnext.hpp"
#include <rti/core/policy/CorePolicy.hpp>

using namespace rti::core::policy;

namespace pyrti {

template<>
void init_class_defs(py::class_<ExclusiveArea>& cls)
{
    cls.def(py::init<>(), "Creates the default policy.")
            .def(py::init<bool>(),
                 py::arg("use_shared_exclusive_area"),
                 "Creates an instance specifying the use of shared or "
                 "exclusive "
                 "area.")
            .def_property(
                    "use_shared_exclusive_area",
                    (bool (ExclusiveArea::*)() const)
                            & ExclusiveArea::use_shared_exclusive_area,
                    (ExclusiveArea & (ExclusiveArea::*) (bool) )
                            & ExclusiveArea::use_shared_exclusive_area,
                    "Whether the Entity is protected by its own exclusive area "
                    "or the "
                    "shared one.")
            .def_static(
                    "shared_ea",
                    &ExclusiveArea::SharedEA,
                    "Returns ExclusiveArea(true)")
            .def_static(
                    "exclusive_ea",
                    &ExclusiveArea::ExclusiveEA,
                    "Returns ExclusiveArea(false)")
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.");
}

template<>
void process_inits<ExclusiveArea>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<ExclusiveArea>(m, "ExclusiveArea");
    });
}

}  // namespace pyrti
