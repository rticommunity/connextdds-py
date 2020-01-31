#include "PyConnext.hpp"
#include <rti/core/policy/CorePolicy.hpp>

using namespace rti::core;

namespace pyrti {

template<>
void init_class_defs(py::class_<EndpointGroup>& cls) {
    cls
        .def(
            py::init<
                const std::string&,
                int32_t
            >(),
            py::arg("role_name"),
            py::arg("quorum_count"),
            "Create an EndpointGroup with the provided parameters."
        )
        .def_property(
            "role_name",
            (std::string (EndpointGroup::*)() const) &EndpointGroup::role_name,
            (EndpointGroup& (EndpointGroup::*)(const std::string&)) &EndpointGroup::role_name,
            "Get/set the EndpointGroup's role name."
        )
        .def_property(
            "quorum_count",
            (int32_t (EndpointGroup::*)() const) &EndpointGroup::quorum_count,
            (EndpointGroup& (EndpointGroup::*)(const int32_t)) &EndpointGroup::quorum_count,
            "Get/set the EndpointGroup's quorum count."
        );
    
    py::implicitly_convertible<std::vector<EndpointGroup>, dds::core::vector<EndpointGroup>>();
}

template<>
void process_inits<EndpointGroup>(py::module& m, ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return init_class<EndpointGroup>(m, "EndpointGroup");
        }
    );
}

}
