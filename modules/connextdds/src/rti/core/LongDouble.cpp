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
#include <sstream>
#include <rti/core/LongDouble.hpp>
#include <pybind11/stl_bind.h>
#include "PyBindVector.hpp"

using namespace rti::core;

namespace pyrti {

#if (RTI_CDR_SIZEOF_LONG_DOUBLE == 16)
#else

template<>
void init_class_defs(py::class_<LongDouble>& cls)
{
    cls.def(py::init<>(), "Creates a LongDouble with value 0.")
            .def(py::init([](std::vector<uint8_t>& bytes) {
                     if (bytes.size() != 16) {
                         throw py::type_error("Size mismatch for LongDouble");
                     }
                     LongDouble ld;
                     auto buf = reinterpret_cast<uint8_t*>(&ld.native().bytes);
                     std::copy(bytes.begin(), bytes.end(), buf);
                     return ld;
                 }),
                 py::arg("byte_sequence"),
                 "Creates a LongDouble from a sequence of bytes.")
            .def(
                    "__getitem__",
                    [](const LongDouble& ld, size_t index) {
                        return static_cast<uint8_t>(ld[index]);
                    },
                    "Get a byte from the LongDouble buffer.")
            .def(
                    "__setitem__",
                    [](LongDouble& ld, size_t index, uint8_t b) {
                        ld[index] = static_cast<char>(b);
                    },
                    "Set a byte in the LongDouble buffer.")
            .def("__str__",
                 [](const LongDouble& ld) {
                     std::ostringstream oss;
                     oss << ld;
                     return oss.str();
                 })
            .def(py::self == py::self, "Test for equality")
            .def(py::self != py::self, "Test for inequality");

    py::implicitly_convertible<std::vector<uint8_t>, LongDouble>();
}

#endif

template<>
void process_inits<LongDouble>(py::module& m, ClassInitList& l)
{
#if (RTI_CDR_SIZEOF_LONG_DOUBLE == 16)
#else
    l.push_back([m]() mutable {
        auto func = init_class<LongDouble>(m, "LongDouble");
        pyrti::bind_vector<rti::core::LongDouble>(m, "Float128Seq");
        py::implicitly_convertible<
                py::iterable,
                std::vector<rti::core::LongDouble>>();
        return func;
    });
#endif
}

}  // namespace pyrti
