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
#include <rti/core/xtypes/DynamicDataProperty.hpp>

using namespace rti::core::xtypes;

namespace pyrti {

template<>
void init_class_defs(py::class_<DynamicDataTypeSerializationProperty>& cls)
{
    cls.def(py::init<>(), "Construct with default settings.")
            .def(py::init<int32_t, int32_t, bool, bool>(),
                 py::arg("max_serialized_size"),
                 py::arg("min_serialized_size"),
                 py::arg("trim_to_size"),
                 py::arg("enable_fast_deserialization"),
                 "Specify all serialization settings at object creation time.")
            .def_property(
                    "max_serialized_size",
                    (uint32_t(DynamicDataTypeSerializationProperty::*)() const)
                            & DynamicDataTypeSerializationProperty::
                                    max_size_serialized,
                    (DynamicDataTypeSerializationProperty
                     & (DynamicDataTypeSerializationProperty::*) (uint32_t))
                            & DynamicDataTypeSerializationProperty::
                                    max_size_serialized,
                    "The maximum number of bytes that objects of a given type "
                    "could "
                    "consume when serialized on the network. [DEPRECATED]")
            .def_property(
                    "min_serialized_size",
                    (uint32_t(DynamicDataTypeSerializationProperty::*)() const)
                            & DynamicDataTypeSerializationProperty::
                                    min_size_serialized,
                    (DynamicDataTypeSerializationProperty
                     & (DynamicDataTypeSerializationProperty::*) (uint32_t))
                            & DynamicDataTypeSerializationProperty::
                                    min_size_serialized,
                    "The minimum number of bytes that objects of a given type "
                    "could "
                    "consume when serialized on the network. [DEPRECATED]")
            .def_property(
                    "trim_to_size",
                    (bool (DynamicDataTypeSerializationProperty::*)() const)
                            & DynamicDataTypeSerializationProperty::
                                    trim_to_size,
                    (DynamicDataTypeSerializationProperty
                     & (DynamicDataTypeSerializationProperty::*) (bool) )
                            & DynamicDataTypeSerializationProperty::
                                    trim_to_size,
                    "Controls the growth of the buffer in a DynamicData "
                    "object.")
            .def_property(
                    "enable_fast_deserialization",
                    (bool (DynamicDataTypeSerializationProperty::*)() const)
                            & DynamicDataTypeSerializationProperty::
                                    enable_fast_deserialization,
                    (DynamicDataTypeSerializationProperty
                     & (DynamicDataTypeSerializationProperty::*) (bool) )
                            & DynamicDataTypeSerializationProperty::
                                    enable_fast_deserialization,
                    "Fast deserialization setting.")
            .def_property_readonly_static(
                    "DEFAULT",
                    [](py::object&) {
                        return DynamicDataTypeSerializationProperty();
                    },
                    "Constant used to specify default configuration.")
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.");
}

template<>
void process_inits<DynamicDataTypeSerializationProperty>(
        py::module& m,
        ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<DynamicDataTypeSerializationProperty>(
                m,
                "DynamicDataTypeSerializationProperty");
    });
}

}  // namespace pyrti
