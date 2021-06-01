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
#include <rti/rti.hpp>

using namespace rti::util;

void init_namespace_rti_util(py::module& m, pyrti::ClassInitList& l)
{
    m.def("sleep",
          &rti::util::sleep,
          py::arg("duration"),
          py::call_guard<py::gil_scoped_release>(),
          "Sleep for a specified duration")
        .def("spin_per_microsecond"
             &rti::util::spin_per_microsecond,
             "Returns the number of spin operations needed"
             "to wait 1 microsecond");
        .def("spin"
             &rti::util::spin,
             "Returns the number of spin operations needed"
             "to wait 1 microsecond");

    init_heap_monitoring(m, l);

#if rti_connext_version_gte(6, 1, 0)
    init_network_capture(m, l);
#endif
}