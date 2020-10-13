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
#include <dds/core/policy/CorePolicy.hpp>

using namespace dds::core::policy;

namespace pyrti {

template<>
void init_class_defs<Partition>(py::class_<Partition>& cls)
{
    cls.def(py::init<>(), "Creates a policy with the default partition.")
            .def(py::init<const std::string&>(),
                 py::arg("partition"),
                 "Creates a policy with a single partition with the specified "
                 "name.")
            .def(py::init<const std::vector<std::string>&>(),
                 py::arg("partitions"),
                 "Creates a policy with the partitions specified in a "
                 "sequence.")
            .def_property(
                    "name",
                    (const std::vector<std::string> (Partition::*)() const)
                            & Partition::name,
                    (Partition
                     & (Partition::*) (const std::vector<std::string>&) )
                            & Partition::name,
                    "The partition names specified in a sequence.")
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.");
}

template<>
void process_inits<Partition>(py::module& m, ClassInitList& l)
{
    l.push_back(
            [m]() mutable { return init_class<Partition>(m, "Partition"); });
}

}  // namespace pyrti
