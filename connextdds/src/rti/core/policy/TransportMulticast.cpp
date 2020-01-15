#include "PyConnext.hpp"
#include <rti/core/policy/CorePolicy.hpp>
#include "PySafeEnum.hpp"

using namespace rti::core;
using namespace rti::core::policy;

template<>
void pyrti::init_class_defs(py::class_<TransportMulticast>& cls) {
    cls
        .def(
            py::init<>(),
            "Creates the default policy."
        )
        .def(
            py::init<
                const std::vector<TransportMulticastSettings>&,
                TransportMulticastKind
            >(),
            py::arg("value"),
            py::arg("kind"),
            "Creates an instance with the speicfied multicast settings."
        )
        .def_property(
            "settings",
            (std::vector<TransportMulticastSettings> (TransportMulticast::*)() const) &TransportMulticast::settings,
            (TransportMulticast& (TransportMulticast::*)(const std::vector<TransportMulticastSettings>&)) &TransportMulticast::settings,
            "A sequence of multicast communications settings."
        )
        .def_property(
            "kind",
            (TransportMulticastKind (TransportMulticast::*)() const) &TransportMulticast::kind,
            (TransportMulticast& (TransportMulticast::*)(TransportMulticastKind)) &TransportMulticast::kind,
            "A value that specifies a way to determine how to obtain the "
            "multicast address."
        )
        .def(
            py::self == py::self,
            "Test for equality."
        )
        .def(
            py::self != py::self,
            "Test for inequality."
        );
}

template<>
void pyrti::init_class_defs(py::class_<TransportMulticastMappingFunction>& cls) {
    cls
        .def(
            py::init<
                const std::string&,
                const std::string&
            >(),
            py::arg("dll"),
            py::arg("function_name"),
            "Create a mapping function instance."
        )
        .def_property(
            "dll",
            [](const TransportMulticastMappingFunction& tmmf) {
                auto val = tmmf.dll();
                if (val.is_set()) return py::cast(val.get());
                return py::cast(nullptr);
            },
            (TransportMulticastMappingFunction& (TransportMulticastMappingFunction::*)(const std::string&)) &TransportMulticastMappingFunction::dll,
            "The name of the dynamic library containing the mapping function."
        )
        .def_property(
            "function_name",
            [](const TransportMulticastMappingFunction& tmmf) {
                auto val = tmmf.function_name();
                if (val.is_set()) return py::cast(val.get());
                return py::cast(nullptr);
            },
            (TransportMulticastMappingFunction& (TransportMulticastMappingFunction::*)(const std::string&)) &TransportMulticastMappingFunction::function_name,
            "The name of the mapping function."
        )
        .def(
            py::self == py::self,
            "Test for equality."
        )
        .def(
            py::self != py::self,
            "Test for inequality."
        );
}

template<>
void pyrti::init_class_defs(py::class_<MulticastMapping>& cls) {
    cls
        .def(
            py::init<>(),
            "Creates the default mapping"
        )
        .def(
            py::init<
                const std::string&,
                const std::string&,
                const TransportMulticastMappingFunction&
            >(),
            py::arg("addresses"),
            py::arg("topic_expression"),
            py::arg("mapping_function"),
            "Creates a mapping with the specified parameters"
        )
        .def_property(
            "addresses",
            [](const MulticastMapping& mm) {
                auto val = mm.addresses();
                if (val.is_set()) return py::cast(val.get());
                return py::cast(nullptr);
            },
            (MulticastMapping& (MulticastMapping::*)(const std::string&)) &MulticastMapping::addresses,
            "Multicast mapping addresses."
        )
        .def_property(
            "topic_expression",
            [](const MulticastMapping& mm) {
                auto val = mm.topic_expression();
                if (val.is_set()) return py::cast(val.get());
                return py::cast(nullptr);
            },
            (MulticastMapping& (MulticastMapping::*)(const std::string&)) &MulticastMapping::topic_expression,
            "The topic expression."
        )
        .def_property(
            "mapping_function",
            (TransportMulticastMappingFunction& (MulticastMapping::*)()) &MulticastMapping::mapping_function,
            (MulticastMapping& (MulticastMapping::*)(const TransportMulticastMappingFunction&)) &MulticastMapping::mapping_function,
            "The mapping function."
        )
        .def(
            py::self == py::self,
            "Test for equality."
        )
        .def(
            py::self != py::self,
            "Test for inequality."
        );
}

template<>
void pyrti::process_inits<TransportMulticast>(py::module& m, pyrti::ClassInitList& l) {
    auto tmk = init_dds_safe_enum<TransportMulticastKind_def>(m, "TransportMulticastKind");

    py::enum_<TransportMulticastKind::type>(tmk, "TransportMulticastKind")
        .value(
            "AUTOMATIC",
            TransportMulticastKind::type::AUTOMATIC,
            "Selects the multicast address automatically."
            "\n\n"
            "NOTE: This setting is required when using the "
            "TransportMulticastMapping."
        )
        .value(
            "UNICAST",
            TransportMulticastKind::type::UNICAST,
            "Selects a unicast-only mode."
        )
        .export_values();

    l.push_back(
        [m]() mutable {
            return pyrti::init_class<TransportMulticast>(m, "TransportMulticast");
        }
    );

    l.push_back(
        [m]() mutable {
            return pyrti::init_class<TransportMulticastMappingFunction>(m, "TransportMulticastMappingFunction");
        }
    );

    l.push_back(
        [m]() mutable {
            return pyrti::init_class<MulticastMapping>(m, "MulticastMapping");
        }
    );
}
