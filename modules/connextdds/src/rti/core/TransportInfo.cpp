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
#include <rti/core/TransportInfo.hpp>
#include "PySafeEnum.hpp"
#include "PySeq.hpp"
#include "PyVector.hpp"

using namespace rti::core;

namespace pyrti {

template<>
void init_class_defs(py::class_<TransportInfo>& cls)
{
    cls.def(py::init<>(), "Creates an instance with default values.")
            .def(py::init<const TransportClassId::type&, int32_t>(),
                 py::arg("class_id"),
                 py::arg("message_size_max"),
                 "Creates an instance with the specified class id and max "
                 "size.")
            .def_property(
                    "class_id",
                    (TransportClassId(TransportInfo::*)() const)
                            & TransportInfo::class_id,
                    (TransportInfo
                     & (TransportInfo::*) (const TransportClassId::type&) )
                            & TransportInfo::class_id,
                    "The class_id identifies the transport associated with the "
                    "message_size_max.")
            .def_property(
                    "message_size_max",
                    (int32_t(TransportInfo::*)() const)
                            & TransportInfo::message_size_max,
                    (TransportInfo & (TransportInfo::*) (int32_t))
                            & TransportInfo::message_size_max,
                    "The maximum size of an RTPS message in bytes that can be "
                    "sent or "
                    "received by the transport plugin identified by the "
                    "class_id.");
}

template<>
void process_inits<TransportInfo>(py::module& m, ClassInitList& l)
{
    init_dds_safe_enum<
            TransportClassId_def>(m, "TransportClassId", [](py::object& o) {
        py::enum_<TransportClassId::type>(o, "TransportClassId")
                .value("INVALID", TransportClassId::type::INVALID, "Invalid.")
                .value("ANY", TransportClassId::type::ANY, "Any.")
                .value("UDPv4", TransportClassId::type::UDPv4, "UDPv4.")
                .value("SHMEM", TransportClassId::type::SHMEM, "Shared memory.")
                .value("SHMEM_510",
                       TransportClassId::type::SHMEM_510,
                       "Shared Memory from 5.1.0 and earlier.")
                .value("INTRA",
                       TransportClassId::type::INTRA,
                       "Intra-participant.")
                .value("UDPv6", TransportClassId::type::UDPv6, "UDPv6.")
                .value("UDPv6_510",
                       TransportClassId::type::UDPv6_510,
                       "UDPv6 for 5.1.0 and earlier.")
                .value("DTLS", TransportClassId::type::DTLS, "DTLS.")
                .value("WAN", TransportClassId::type::WAN, "WAN.")
                .value("TCPV4_LAN",
                       TransportClassId::type::TCPV4_LAN,
                       "TCPv4 LAN mode.")
                .value("TCPV4_WAN",
                       TransportClassId::type::TCPV4_WAN,
                       "TCPv4 WAN mode.")
                .value("TLSV4_LAN",
                       TransportClassId::type::TLSV4_LAN,
                       "TCPv4 LAN mode with TLS.")
                .value("TLSV4_WAN",
                       TransportClassId::type::TLSV4_WAN,
                       "TCPv4 WAN mode with TLS.")
                .value("RESERVED_RANGE",
                       TransportClassId::type::RESERVED_RANGE,
                       "eserved for additional user-defined transport plugins.")
                .export_values();
    });

    l.push_back([m, &l]() mutable {
        auto func = init_class_with_seq<TransportInfo>(m, "TransportInfo");
        init_dds_vector_nonbuffer_class<TransportInfo>(
                m,
                "TransportInfoVector",
                l);
        return func;
    });
}

}  // namespace pyrti
