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
#include "PyDomainParticipantListener.hpp"

namespace pyrti {

template<>
void init_class_defs(py::class_<
                     PyDomainParticipantListener,
                     PyPublisherListener,
                     PySubscriberListener,
                     PyAnyTopicListener,
                     PyDomainParticipantListenerTrampoline<>,
                     std::shared_ptr<PyDomainParticipantListener>>& cls)
{
    cls.def(py::init<>());
    cls.def("on_invalid_local_identity_status_advance_notice",
            (void(PyDomainParticipantListener::*)(
                    PyDomainParticipant&,
                    const rti::core::status::
                            InvalidLocalIdentityAdvanceNoticeStatus&))
                    & PyDomainParticipantListener::
                            on_invalid_local_identity_status_advance_notice,
            "On invalid local identity status advance notice callback");
}

template<>
void init_class_defs(py::class_<
                     PyNoOpDomainParticipantListener,
                     PyDomainParticipantListener,
                     PyNoOpDomainParticipantListenerTrampoline<>,
                     std::shared_ptr<PyNoOpDomainParticipantListener>>& cls)
{
    cls.def(py::init<>());
    cls.def("on_invalid_local_identity_status_advance_notice",
            (void(PyNoOpDomainParticipantListener::*)(
                    PyDomainParticipant&,
                    const rti::core::status::
                            InvalidLocalIdentityAdvanceNoticeStatus&))
                    & PyNoOpDomainParticipantListener::
                            on_invalid_local_identity_status_advance_notice,
            "On invalid local identity status advance notice callback");
}

template<>
void process_inits<dds::domain::DomainParticipantListener>(
        py::module& m,
        ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<
                PyDomainParticipantListener,
                PyPublisherListener,
                PySubscriberListener,
                PyAnyTopicListener,
                PyDomainParticipantListenerTrampoline<>,
                std::shared_ptr<PyDomainParticipantListener>>(
                    m,
                    "DomainParticipantListener");
    });

    l.push_back([m]() mutable {
        return init_class<
                PyNoOpDomainParticipantListener,
                PyDomainParticipantListener,
                PyNoOpDomainParticipantListenerTrampoline<>,
                std::shared_ptr<PyNoOpDomainParticipantListener>>(
                    m,
                    "NoOpDomainParticipantListener");
    });
}

}  // namespace pyrti
