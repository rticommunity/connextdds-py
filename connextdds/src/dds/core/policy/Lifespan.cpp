#include "PyConnext.hpp"
#include <dds/core/policy/CorePolicy.hpp>

using namespace dds::core::policy;

namespace pyrti {

template<>
void init_class_defs(py::class_<Lifespan>& cls) {
    cls
        .def(
            py::init<>(),
            "Creates the default policy with an infinite lifespan."
        )
        .def(
            py::init<
                const dds::core::Duration&
            >(),
            py::arg("duration"),
            "Creates a policy with the specified lifespan duration."
        )
        .def_property(
            "duration",
            (const dds::core::Duration (Lifespan::*)() const) &Lifespan::duration,
            (Lifespan& (Lifespan::*)(const dds::core::Duration&)) &Lifespan::duration,
            "Sets the maximum duration for which the data is valid."
            "\n\n"
            "This property uses value semantics."
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
void process_inits<Lifespan>(py::module& m, ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return init_class<Lifespan>(m, "Lifespan");
        }
    );
}

}
