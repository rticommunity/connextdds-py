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

using namespace rti::core;
using namespace rti::core::policy;

namespace pyrti {

template<>
void init_class_defs(py::class_<MultiChannel>& cls)
{
    cls.def(py::init<>(), "Creates the default policy.")
            .def(py::init<
                         const std::vector<ChannelSettings>&,
                         const std::string&>(),
                 py::arg("channels"),
                 py::arg_v(
                         "filter_name",
                         rti::topic::stringmatch_filter_name(),
                         "Filter.stringmatch_filter_name"),
                 "Creates an instance with the specified channels and filter "
                 "name.")
            .def_property(
                    "channels",
                    (std::vector<ChannelSettings>(MultiChannel::*)() const)
                            & MultiChannel::channels,
                    (MultiChannel
                     & (MultiChannel::*) (const std::vector<ChannelSettings>&) )
                            & MultiChannel::channels,
                    "A sequence of ChannelSettings used to configure the "
                    "channels' "
                    "properties. If the length of the sequence is zero, the "
                    "QoS "
                    "policy will be ignored."
                    "\n\n"
                    "This property's getter returns a deep copy.")
            .def_property(
                    "filter_name",
                    (std::string(MultiChannel::*)() const)
                            & MultiChannel::filter_name,
                    (MultiChannel & (MultiChannel::*) (const std::string&) )
                            & MultiChannel::filter_name,
                    "Name of the filter class used to describe the filter "
                    "expressions "
                    "of a MultiChannel DataWriter.")
            .def_property_readonly_static(
                    "sql_filter_name",
                    [](py::object&) { return rti::topic::sql_filter_name(); },
                    "The name of the SQL filter.")
            .def_property_readonly_static(
                    "stringmatch_filter_name",
                    [](py::object&) {
                        return rti::topic::stringmatch_filter_name();
                    },
                    "The name of the string match filter.")
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.");
}

template<>
void process_inits<MultiChannel>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<MultiChannel>(m, "MultiChannel");
    });
}

}  // namespace pyrti
