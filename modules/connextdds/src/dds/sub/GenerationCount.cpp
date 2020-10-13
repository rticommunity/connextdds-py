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
#include <dds/sub/SampleInfo.hpp>

using namespace dds::sub;

namespace pyrti {

template<>
void init_class_defs(py::class_<GenerationCount>& cls)
{
    cls.def(py::init<>(), "Create a default GenerationCount object.")
            .def(py::init<int32_t, int32_t>(),
                 py::arg("disposed_count"),
                 py::arg("no_writers_count"),
                 "Create a GenerationCount object with the provided "
                 "disposed_count "
                 "and no_writers count.")
            .def_property_readonly(
                    "disposed",
                    &GenerationCount::disposed,
                    "Get the disposed generation count.")
            .def_property_readonly(
                    "no_writers",
                    &GenerationCount::no_writers,
                    "Get the no_writers generation count.");
}

template<>
void process_inits<GenerationCount>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<GenerationCount>(m, "GenerationCount");
    });
}

}  // namespace pyrti
