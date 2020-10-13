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
#include "PySafeEnum.hpp"
#include "PySeq.hpp"

using namespace rti::core;
using namespace rti::core::policy;

namespace pyrti {

template<>
void init_class_defs(py::class_<TransportMulticast>& cls)
{
    cls.def(py::init<>(), "Creates the default policy.")
            .def(py::init<
                         const std::vector<TransportMulticastSettings>&,
                         TransportMulticastKind>(),
                 py::arg("value"),
                 py::arg("kind"),
                 "Creates an instance with the speicfied multicast settings.")
            .def_property(
                    "settings",
                    (std::vector<TransportMulticastSettings>(
                            TransportMulticast::*)() const)
                            & TransportMulticast::settings,
                    (TransportMulticast
                     & (TransportMulticast::*) (const std::vector<
                                                TransportMulticastSettings>&) )
                            & TransportMulticast::settings,
                    "A sequence of multicast communications settings.")
            .def_property(
                    "kind",
                    (TransportMulticastKind(TransportMulticast::*)() const)
                            & TransportMulticast::kind,
                    (TransportMulticast
                     & (TransportMulticast::*) (TransportMulticastKind))
                            & TransportMulticast::kind,
                    "A value that specifies a way to determine how to obtain "
                    "the "
                    "multicast address.")
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.");
}

template<>
void init_class_defs(py::class_<TransportMulticastMappingFunction>& cls)
{
    cls.def(py::init<const std::string&, const std::string&>(),
            py::arg("dll"),
            py::arg("function_name"),
            "Create a mapping function instance.")
            .def_property(
                    "dll",
                    (rti::core::optional_value<std::string>(
                            TransportMulticastMappingFunction::*)() const)
                            & TransportMulticastMappingFunction::dll,
                    (TransportMulticastMappingFunction
                     & (TransportMulticastMappingFunction::*) (const std::
                                                                       string&) )
                            & TransportMulticastMappingFunction::dll,
                    "The name of the dynamic library containing the mapping "
                    "function.")
            .def_property(
                    "function_name",
                    (rti::core::optional_value<std::string>(
                            TransportMulticastMappingFunction::*)() const)
                            & TransportMulticastMappingFunction::function_name,
                    (TransportMulticastMappingFunction
                     & (TransportMulticastMappingFunction::*) (const std::
                                                                       string&) )
                            & TransportMulticastMappingFunction::function_name,
                    "The name of the mapping function.")
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.");
}

template<>
void init_class_defs(py::class_<MulticastMapping>& cls)
{
    cls.def(py::init<>(), "Creates the default mapping")
            .def(py::init<
                         const std::string&,
                         const std::string&,
                         const TransportMulticastMappingFunction&>(),
                 py::arg("addresses"),
                 py::arg("topic_expression"),
                 py::arg("mapping_function"),
                 "Creates a mapping with the specified parameters")
            .def_property(
                    "addresses",
                    (rti::core::optional_value<std::string>(
                            MulticastMapping::*)() const)
                            & MulticastMapping::addresses,
                    (MulticastMapping
                     & (MulticastMapping::*) (const std::string&) )
                            & MulticastMapping::addresses,
                    "Multicast mapping addresses.")
            .def_property(
                    "topic_expression",
                    (rti::core::optional_value<std::string>(
                            MulticastMapping::*)() const)
                            & MulticastMapping::topic_expression,
                    (MulticastMapping
                     & (MulticastMapping::*) (const std::string&) )
                            & MulticastMapping::topic_expression,
                    "The topic expression.")
            .def_property(
                    "mapping_function",
                    (TransportMulticastMappingFunction
                     & (MulticastMapping::*) ())
                            & MulticastMapping::mapping_function,
                    (MulticastMapping
                     & (MulticastMapping::*) (const TransportMulticastMappingFunction&) )
                            & MulticastMapping::mapping_function,
                    "The mapping function.")
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.");
}

template<>
void init_class_defs(py::class_<TransportMulticastMapping>& cls)
{
    cls.def(py::init<>(), "Creates the default policy.")
            .def(py::init<const std::vector<MulticastMapping>&>(),
                 py::arg("mappings"),
                 "Creates an object with with specified mappings.")
            .def_property(
                    "mappings",
                    (std::vector<MulticastMapping>(
                            TransportMulticastMapping::*)() const)
                            & TransportMulticastMapping::mappings,
                    (TransportMulticastMapping
                     & (TransportMulticastMapping::*) (const std::vector<
                                                       MulticastMapping>&) )
                            & TransportMulticastMapping::mappings,
                    "A sequence of transport multicast mappings.")
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.");
}

template<>
void process_inits<TransportMulticast>(py::module& m, ClassInitList& l)
{
    init_dds_safe_enum<TransportMulticastKind_def>(
            m,
            "TransportMulticastKind",
            [](py::object& o) {
                py::enum_<TransportMulticastKind::type>(o, "Enum")
                        .value("AUTOMATIC",
                               TransportMulticastKind::type::AUTOMATIC,
                               "Selects the multicast address automatically."
                               "\n\n"
                               "NOTE: This setting is required when using the "
                               "TransportMulticastMapping.")
                        .value("UNICAST",
                               TransportMulticastKind::type::UNICAST,
                               "Selects a unicast-only mode.")
                        .export_values();
            });

    l.push_back([m]() mutable {
        return init_class_with_seq<TransportMulticast>(m, "TransportMulticast");
    });

    l.push_back([m]() mutable {
        return init_class<TransportMulticastMappingFunction>(
                m,
                "TransportMulticastMappingFunction");
    });

    l.push_back([m]() mutable {
        return init_class_with_seq<MulticastMapping>(m, "MulticastMapping");
    });

    l.push_back([m]() mutable {
        return init_class<TransportMulticastMapping>(
                m,
                "TransportMulticastMapping");
    });
}

}  // namespace pyrti
