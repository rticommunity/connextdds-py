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
#include <dds/core/Exception.hpp>

using namespace dds::core;

namespace pyrti {

template<>
void process_inits<Exception>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() {
        auto ex = py::register_exception<Exception>(m, "Exception");
        py::register_exception<AlreadyClosedError>(
                m,
                "AlreadyClosedError",
                ex.ptr());
        py::register_exception<Error>(m, "Error", ex.ptr());
        py::register_exception<IllegalOperationError>(
                m,
                "IllegalOperationError",
                ex.ptr());
        py::register_exception<ImmutablePolicyError>(
                m,
                "ImmutablePolicyError",
                ex.ptr());
        py::register_exception<InconsistentPolicyError>(
                m,
                "InconsistentPolicyError",
                ex.ptr());
        py::register_exception<InvalidArgumentError>(
                m,
                "InvalidArgumentError",
                ex.ptr());
        py::register_exception<InvalidDowncastError>(
                m,
                "InvalidDowncastError",
                ex.ptr());
#if rti_connext_version_gte(6, 0, 0)
        py::register_exception<NotAllowedBySecurityError>(
                m,
                "NotAllowedBySecurityError",
                ex.ptr());
#endif
        py::register_exception<NotEnabledError>(m, "NotEnabledError", ex.ptr());
        py::register_exception<NullReferenceError>(
                m,
                "NullReferenceError",
                ex.ptr());
        py::register_exception<OutOfResourcesError>(
                m,
                "OutOfResourcesError",
                ex.ptr());
        py::register_exception<PreconditionNotMetError>(
                m,
                "PreconditionNotMetError",
                ex.ptr());
        py::register_exception<TimeoutError>(m, "TimeoutError", ex.ptr());
        py::register_exception<UnsupportedError>(
                m,
                "UnsupportedError",
                ex.ptr());
        return ([]() {});
    });
}

}  // namespace pyrti
