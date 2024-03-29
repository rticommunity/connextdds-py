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
#include <rti/core/Cookie.hpp>
#include "PyVector.hpp"
#include "PySeq.hpp"

using namespace rti::core;

namespace pyrti {

template<>
void init_class_defs(py::class_<Cookie>& cls)
{
    cls.def(py::init<>(), "Creates an empty Cookie.");

    cls.def(py::init<dds::core::vector<uint8_t>&>(), py::arg("bytes"), "Cookie from bytes.")
            .def_property_readonly(
                    "value",
                    [](Cookie& c) { return std::vector<uint8_t>(c.value()); },
                    "Retrieve the bytes of the Cookie's contents.")
            .def(py::self == py::self, "Compare Cookies for equality")
            .def(py::self != py::self, "Compare Cookies for inequality")
            .doc() =
            "Unique identifier for a written data sample in the form of a "
            "sequence of bytes.";
}

template<>
void process_inits<Cookie>(py::module& m, ClassInitList& l)
{
    l.push_back([m, &l]() mutable {
        auto func = init_class_with_seq<Cookie>(m, "Cookie");
        init_dds_vector_nonbuffer_class<Cookie>(m, "CookieVector", l);
        return func;
    });
}

}  // namespace pyrti
