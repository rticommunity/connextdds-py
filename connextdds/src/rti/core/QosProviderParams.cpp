#include "PyConnext.hpp"
#include <rti/core/QosProviderParams.hpp>

using namespace rti::core;

namespace pyrti {

template <> void init_class_defs(py::class_<QosProviderParams> &cls) {
  cls.def(py::init<>(), "Create a QosProviderParams with default settings.")
      .def_property("string_profile",
                    (std::vector<std::string>(QosProviderParams::*)() const) &
                        QosProviderParams::string_profile,
                    (QosProviderParams &
                     (QosProviderParams::*)(const std::vector<std::string> &)) &
                        QosProviderParams::string_profile,
                    "The current list of string profiles stored by this "
                    "QosProviderParams.")
      .def_property("url_profile",
                    (std::vector<std::string>(QosProviderParams::*)() const) &
                        QosProviderParams::url_profile,
                    (QosProviderParams &
                     (QosProviderParams::*)(const std::vector<std::string> &)) &
                        QosProviderParams::url_profile,
                    "The current list of URL groups stored by this "
                    "QosProviderParams.")
      .def_property("ignore_user_profile",
                    (bool (QosProviderParams::*)() const) &
                        QosProviderParams::ignore_user_profile,
                    (QosProviderParams & (QosProviderParams::*)(bool)) &
                        QosProviderParams::ignore_user_profile,
                    "Choose whether or not to ignore USER_QOS_PROFILES.xml.")
      .def_property("ignore_environment_profile",
                    (bool (QosProviderParams::*)() const) &
                        QosProviderParams::ignore_environment_profile,
                    (QosProviderParams & (QosProviderParams::*)(bool)) &
                        QosProviderParams::ignore_environment_profile,
                    "Choose whether or not to ignore the NDDS_QOS_PROFILES.")
      .def_property("ignore_resource_profile",
                    (bool (QosProviderParams::*)() const) &
                        QosProviderParams::ignore_resource_profile,
                    (QosProviderParams & (QosProviderParams::*)(bool)) &
                        QosProviderParams::ignore_resource_profile,
                    "Choose whether or not to ignore NDDS_QOS_PROFILES.xml.")
      .def(py::self == py::self, "Test for equality.")
      .def(py::self != py::self, "Test for inequality.");
}

template <>
void process_inits<QosProviderParams>(py::module &m, ClassInitList &l) {
  l.push_back([m]() mutable {
    return init_class<QosProviderParams>(m, "QosProviderParams");
  });
}

} // namespace pyrti
