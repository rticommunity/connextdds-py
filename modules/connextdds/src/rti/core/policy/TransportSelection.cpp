/*
 * (c) 2020 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 *
 * RTI grants Licensee a license to use, modify, compile, and create derivative
 * works of the Software solely for use with RTI products.  The Software is
 * provided "as is", with no warranty of any type, including any warranty for
 * fitness for any purpose. RTI is under no obligation to maintain or support
 * the Software.  RTI shall not be liable for any incidental or consequential
 * damages arising out of the use or inability to use the software.
 */

#include "PyConnext.hpp"
#include <rti/core/policy/CorePolicy.hpp>

using namespace rti::core::policy;

namespace pyrti {

template<>
void init_class_defs(py::class_<TransportSelection>& cls)
{
    cls.def(py::init<>(), "Creates the default policy.")
            .def(py::init<const std::vector<std::string>&>(),
                 py::arg("enabled_transports"),
                 "Creates an instance with the sepcified transport aliases.")
            .def_property(
                    "enabled_transports",
                    (std::vector<std::string>(TransportSelection::*)() const)
                            & TransportSelection::enabled_transports,
                    (TransportSelection
                     & (TransportSelection::*) (const std::vector<
                                                std::string>&) )
                            & TransportSelection::enabled_transports,
                    "A sequence of transport aliases that specifies the "
                    "transport "
                    "instances available for use by the entity."
                    "\n\n"
                    "This property's getter returns a deep copy.")
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.");
}

template<>
void process_inits<TransportSelection>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<TransportSelection>(m, "TransportSelection");
    });
}

}  // namespace pyrti
