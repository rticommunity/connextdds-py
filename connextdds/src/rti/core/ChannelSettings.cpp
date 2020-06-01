#include "PyConnext.hpp"
#include "PySeq.hpp"
#include <rti/core/policy/CorePolicy.hpp>

using namespace rti::core;

namespace pyrti {

template<>
void init_class_defs(py::class_<ChannelSettings>& cls) {
    cls
        .def(
            py::init<
                const std::vector<TransportMulticastSettings>&,
                const std::string&,
                int32_t
            >(),
            py::arg("multicast_settings"),
            py::arg("filter_expression"),
            py::arg("priority"),
            "Creates an instance with the specified multicast settings, "
            "filter expression and priority."
        )
        .def_property(
            "multicast_settings",
            (std::vector<TransportMulticastSettings> (ChannelSettings::*)() const) &ChannelSettings::multicast_settings,
            (ChannelSettings& (ChannelSettings::*)(const std::vector<TransportMulticastSettings>&)) &ChannelSettings::multicast_settings,
            "A sequence of TransportMulticastSettings used to configure the "
            "multicast addresses associated with a channel."
        )
        .def_property(
            "filter_expression",
            (std::string (ChannelSettings::*)() const) &ChannelSettings::filter_expression,
            (ChannelSettings& (ChannelSettings::*)(const std::string&)) &ChannelSettings::filter_expression,
            "A logical expression used to determine the data that will be "
            "published in the channel."
        )
        .def_property(
            "priority",
            (int32_t (ChannelSettings::*)() const) &ChannelSettings::priority,
            (ChannelSettings& (ChannelSettings::*)(int32_t)) &ChannelSettings::priority,
            "Publication priority."
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
void process_inits<ChannelSettings>(py::module& m, ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return init_class_with_seq<ChannelSettings>(m, "ChannelSettings");
        }
    );
}

}
