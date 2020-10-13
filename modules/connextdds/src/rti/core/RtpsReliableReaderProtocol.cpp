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

using namespace dds::core;
using namespace rti::core;
using namespace rti::core::policy;

namespace pyrti {

template<>
void init_class_defs(py::class_<RtpsReliableReaderProtocol>& cls)
{
    cls.def(py::init<>(),
            "Create an RtpsReliableReaderProtocol policy with default "
            "settings.")
            .def_property(
                    "min_heartbeat_response_delay",
                    (Duration(RtpsReliableReaderProtocol::*)() const)
                            & RtpsReliableReaderProtocol::
                                    min_heartbeat_response_delay,
                    (RtpsReliableReaderProtocol
                     & (RtpsReliableReaderProtocol::*) (const Duration&) )
                            & RtpsReliableReaderProtocol::
                                    min_heartbeat_response_delay,
                    "Get/set the minimum time to respond with an ACKNACK to a "
                    "DataWriter's heartbeat.")
            .def_property(
                    "max_heartbeat_response_delay",
                    (Duration(RtpsReliableReaderProtocol::*)() const)
                            & RtpsReliableReaderProtocol::
                                    max_heartbeat_response_delay,
                    (RtpsReliableReaderProtocol
                     & (RtpsReliableReaderProtocol::*) (const Duration&) )
                            & RtpsReliableReaderProtocol::
                                    max_heartbeat_response_delay,
                    "Get/set the maximum time to respond with an ACKNACK to a "
                    "DataWriter's heartbeat.")
            .def_property(
                    "heartbeat_suppression_duration",
                    (Duration(RtpsReliableReaderProtocol::*)() const)
                            & RtpsReliableReaderProtocol::
                                    heartbeat_suppression_duration,
                    (RtpsReliableReaderProtocol
                     & (RtpsReliableReaderProtocol::*) (const Duration&) )
                            & RtpsReliableReaderProtocol::
                                    heartbeat_suppression_duration,
                    "Get/set the duration to ignore consecutive heartbeats "
                    "from a "
                    "DataWriter.")
            .def_property(
                    "nack_period",
                    (Duration(RtpsReliableReaderProtocol::*)() const)
                            & RtpsReliableReaderProtocol::nack_period,
                    (RtpsReliableReaderProtocol
                     & (RtpsReliableReaderProtocol::*) (const Duration&) )
                            & RtpsReliableReaderProtocol::nack_period,
                    "Get/set the period at which to send NACKs for receiving "
                    "historical data.")
            .def_property(
                    "receive_window_size",
                    (int32_t(RtpsReliableReaderProtocol::*)() const)
                            & RtpsReliableReaderProtocol::receive_window_size,
                    (RtpsReliableReaderProtocol
                     & (RtpsReliableReaderProtocol::*) (const int32_t))
                            & RtpsReliableReaderProtocol::receive_window_size,
                    "Get/set the number of received out-of-order samples a "
                    "reader can "
                    "keep at a time.")
            .def_property(
                    "round_trip_time",
                    (Duration(RtpsReliableReaderProtocol::*)() const)
                            & RtpsReliableReaderProtocol::round_trip_time,
                    (RtpsReliableReaderProtocol
                     & (RtpsReliableReaderProtocol::*) (const Duration&) )
                            & RtpsReliableReaderProtocol::round_trip_time,
                    "Get/set the estimated duration from sending a NACK to "
                    "receiving "
                    "a repair of a sample.")
            .def_property(
                    "app_ack_period",
                    (Duration(RtpsReliableReaderProtocol::*)() const)
                            & RtpsReliableReaderProtocol::app_ack_period,
                    (RtpsReliableReaderProtocol
                     & (RtpsReliableReaderProtocol::*) (const Duration&) )
                            & RtpsReliableReaderProtocol::app_ack_period,
                    "Get/set the period at which application-level "
                    "acknowledgment "
                    "messages are sent.")
            .def_property(
                    "samples_per_app_ack",
                    (int32_t(RtpsReliableReaderProtocol::*)() const)
                            & RtpsReliableReaderProtocol::samples_per_app_ack,
                    (RtpsReliableReaderProtocol
                     & (RtpsReliableReaderProtocol::*) (const int32_t))
                            & RtpsReliableReaderProtocol::samples_per_app_ack,
                    "Get/set the minimum number of samples acknowledged by one "
                    "application-level acknowledgment message.")
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.");
}

template<>
void process_inits<RtpsReliableReaderProtocol>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<RtpsReliableReaderProtocol>(
                m,
                "RtpsReliableReaderProtocol");
    });
}

}  // namespace pyrti
