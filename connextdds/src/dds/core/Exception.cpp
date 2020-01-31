#include "PyConnext.hpp"
#include <dds/core/Exception.hpp>

using namespace dds::core;

namespace pyrti {

template<>
void process_inits<Exception>(py::module& m, ClassInitList& l) {
    l.push_back(
        [m]() {
            auto ex = py::register_exception<Exception>(
                m,
                "Exception"
            );
            py::register_exception<AlreadyClosedError>(
                m,
                "AlreadyClosedError",
                ex.ptr()
            );
            py::register_exception<Error>(
                m,
                "Error",
                ex.ptr()
            );
            py::register_exception<IllegalOperationError>(
                m,
                "IllegalOperationError",
                ex.ptr()
            );
            py::register_exception<ImmutablePolicyError>(
                m,
                "ImmutablePolicyError",
                ex.ptr()
            );
            py::register_exception<InconsistentPolicyError>(
                m,
                "InconsistentPolicyError",
                ex.ptr()
            );
            py::register_exception<InvalidArgumentError>(
                m,
                "InvalidArgumentError",
                ex.ptr()
            );
            py::register_exception<InvalidDowncastError>(
                m,
                "InvalidDowncastError",
                ex.ptr()
            );
            py::register_exception<NotAllowedBySecurityError>(
                m,
                "NotAllowedBySecurityError",
                ex.ptr()
            );
            py::register_exception<NotEnabledError>(
                m,
                "NotEnabledError",
                ex.ptr()
            );
            py::register_exception<NullReferenceError>(
                m,
                "NullReferenceError",
                ex.ptr()
            );
            py::register_exception<OutOfResourcesError>(
                m,
                "OutOfResourcesError",
                ex.ptr()
            );
            py::register_exception<PreconditionNotMetError>(
                m,
                "PreconditionNotMetError",
                ex.ptr()
            );
            py::register_exception<TimeoutError>(
                m,
                "TimeoutError",
                ex.ptr()
            );
            py::register_exception<UnsupportedError>(
                m,
                "UnsupportedError",
                ex.ptr()
            );
            return (
                []() {}     
            );
        }
    ); 
    
}

}
