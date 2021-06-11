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
#include "PyMaskType.hpp"

using namespace rti::core::policy;

namespace pyrti {

template<>
void init_class_defs(py::class_<TransportBuiltinMask>& cls)
{
    cls.def_property_readonly_static(
                "ALL",
                [](py::object&) {
                    return TransportBuiltinMask::all();
                },
                "All bits are set.")
            .def_property_readonly_static(
                    "NONE",
                    [](py::object&) {
                        return TransportBuiltinMask::none();
                    },
                    "No bits are set.")
            .def_property_readonly_static(
                    "SHMEM",
                    [](py::object&) {
                        return TransportBuiltinMask::shmem();
                    },
                    "Selects the built-in shared-memory transport.")
            .def_property_readonly_static(
                    "UDPv4",
                    [](py::object&) {
                        return TransportBuiltinMask::udpv4();
                    },
                    "Selects the built-in UDPv4 transport.")
            .def_property_readonly_static(
                    "UDPv6",
                    [](py::object&) {
                        return TransportBuiltinMask::udpv6();
                    },
                    "Selects the built-in UDPv6 transport.");
}

template<>
void init_class_defs(py::class_<TransportBuiltin>& cls)
{
    cls.def(py::init<>(), "Creates the default policy.")
            .def(py::init<const TransportBuiltinMask&>(),
                 py::arg("mask"),
                 "Creates an instnace with the transport selection that the "
                 "mask "
                 "specifies.")
            .def_property(
                    "mask",
                    (TransportBuiltinMask(TransportBuiltin::*)() const)
                            & TransportBuiltin::mask,
                    (TransportBuiltin
                     & (TransportBuiltin::*) (TransportBuiltinMask))
                            & TransportBuiltin::mask,
                    "The selected transports through a mask.")
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.")
            .def_property_readonly_static(
                    "all",
                    [](py::object&) {
                        return TransportBuiltin::All();
                    },
                    "Creates a policy that selects TransportBuiltinMask.all()")
            .def_property_readonly_static(
                    "none",
                    [](py::object&) {
                        return TransportBuiltin::None();
                    },
                    "Creates a policy that selects TransportBuiltinMask.none()")
            .def_property_readonly_static(
                    "shmem",
                    [](py::object&) {
                        return TransportBuiltin::Shmem();
                    },
                    "Creates a policy that selects "
                    "TransportBuiltinMask.shmem()")
            .def_property_readonly_static(
                    "udpv4",
                    [](py::object&) {
                        return TransportBuiltin::UDPv4();
                    },
                    "Creates a policy that selects "
                    "TransportBuiltinMask.udpv4()")
            .def_property_readonly_static(
                    "udpv6",
                    [](py::object&) {
                        return TransportBuiltin::UDPv6();
                    },
                    "Creates a policy that selects "
                    "TransportBuiltinMask.udpv6()");
}

template<>
void process_inits<TransportBuiltin>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        auto cls = init_mask_type<TransportBuiltinMask, uint64_t>(
                m,
                "TransportBuiltinMask",
                "Create a TransportBuiltinMask equivalent to "
                "TransportBuiltinMask.none()");
        return [cls]() mutable { init_class_defs<TransportBuiltinMask>(cls); };
    });

    l.push_back([m]() mutable {
        return init_class<TransportBuiltin>(m, "TransportBuiltin");
    });
}

}  // namespace pyrti
