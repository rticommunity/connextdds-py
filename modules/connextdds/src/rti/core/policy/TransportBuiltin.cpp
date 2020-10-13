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
    cls.def_static("all", &TransportBuiltinMask::all, "All bits are set.")
            .def_static("none", &TransportBuiltinMask::none, "No bits are set.")
            .def_static(
                    "shmem",
                    &TransportBuiltinMask::shmem,
                    "Selects the built-in shared-memory transport.")
            .def_static(
                    "udpv4",
                    &TransportBuiltinMask::udpv4,
                    "Selects the built-in UDPv4 transport.")
            .def_static(
                    "udpv6",
                    &TransportBuiltinMask::udpv6,
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
            .def_static(
                    "all",
                    TransportBuiltin::All,
                    "Creates a policy that selects TransportBuiltinMask.all()")
            .def_static(
                    "none",
                    TransportBuiltin::None,
                    "Creates a policy that selects TransportBuiltinMask.none()")
            .def_static(
                    "shmem",
                    TransportBuiltin::Shmem,
                    "Creates a policy that selects "
                    "TransportBuiltinMask.shmem()")
            .def_static(
                    "udpv4",
                    TransportBuiltin::UDPv4,
                    "Creates a policy that selects "
                    "TransportBuiltinMask.udpv4()")
            .def_static(
                    "udpv6",
                    TransportBuiltin::UDPv6,
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
