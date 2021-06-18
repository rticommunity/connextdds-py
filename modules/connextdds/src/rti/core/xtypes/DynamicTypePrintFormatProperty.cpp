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

#if rti_connext_version_gte(6, 0, 1)

#include <rti/core/xtypes/DynamicTypePrintFormat.hpp>

using namespace rti::core::xtypes;

namespace pyrti {

template<>
void init_class_defs(py::class_<DynamicTypePrintFormatProperty>& cls)
{
    cls.def(py::init<uint32_t, bool>(),
                py::arg("indent") = 0,
                py::arg("print_ordinals") = false,
                "Construct with default settings.")
            .def_property(
                    "indent",
                    (uint32_t(DynamicTypePrintFormatProperty::*)() const)
                            & DynamicTypePrintFormatProperty::indent,
                    (DynamicTypePrintFormatProperty
                     & (DynamicTypePrintFormatProperty::*) (uint32_t))
                            & DynamicTypePrintFormatProperty::indent,
                    "The amount of additional indent to be included when "
                    "converting a DynamicType to a string.")
            .def_property(
                    "min_serialized_size",
                    (bool(DynamicTypePrintFormatProperty::*)() const)
                            & DynamicTypePrintFormatProperty::print_ordinals,
                    (DynamicTypePrintFormatProperty
                     & (DynamicTypePrintFormatProperty::*) (bool))
                            & DynamicTypePrintFormatProperty::print_ordinals,
                    "The minimum number of bytes that objects of a given type "
                    "could "
                    "consume when serialized on the network. [DEPRECATED]")
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.");
}

template<>
void process_inits<DynamicTypePrintFormatProperty>(
        py::module& m,
        ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<DynamicTypePrintFormatProperty>(
                m,
                "DynamicTypePrintFormatProperty");
    });
}

}  // namespace pyrti

#endif