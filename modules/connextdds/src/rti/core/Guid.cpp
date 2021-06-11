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
#include <rti/core/Guid.hpp>

using namespace rti::core;

namespace pyrti {

template<>
void init_class_defs(py::class_<Guid>& cls)
{
    cls.def(py::init<>(), "Create a default Guid (equivalent to unknown Guid).")
            .def("__getitem__",
                 [](Guid& g, uint32_t index) { return g[index]; })
            .def("__setitem__",
                 [](Guid& g, uint32_t index, uint8_t value) {
                     return g[index] = value;
                 })
            .def(py::self == py::self, "Compare Guids for equality.")
            .def(py::self != py::self, "Compare Guids for inequality.")
            .def(py::self < py::self, "Compare Guids.")
            .def(py::self <= py::self, "Compare Guids.")
            .def(py::self > py::self, "Compare Guids.")
            .def(py::self >= py::self, "Compare Guids.")
            .def("__len__", [](Guid& g) { return g.LENGTH; })
            .def("__str__",
                 [](Guid& g) {
                     std::ostringstream oss;
                     oss << g;
                     return oss.str();
                 })
            .def_property_readonly_static(
                    "unknown",
                    [](py::object&) {
                        return Guid::unknown();
                    },
                    "Create an unknown Guid.")
            .def_property_readonly_static(
                    "automatic",
                    [](py::object&) {
                        return Guid::automatic();
                    },
                    "Construct a Guid indicating that RTI Connext should "
                    "choose an "
                    "appropriate virtual GUID.");
}

template<>
void process_inits<Guid>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable { return init_class<Guid>(m, "Guid"); });
}

}  // namespace pyrti
