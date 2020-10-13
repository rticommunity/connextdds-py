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
                     PyDomainParticipantListenerTrampoline<>>& cls)
{
    cls.def(py::init<>());
}

template<>
void init_class_defs(py::class_<
                     PyNoOpDomainParticipantListener,
                     PyDomainParticipantListener,
                     PyNoOpDomainParticipantListenerTrampoline<>>& cls)
{
    cls.def(py::init<>());
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
                PyDomainParticipantListenerTrampoline<>>(
                m,
                "DomainParticipantListener");
    });

    l.push_back([m]() mutable {
        return init_class<
                PyNoOpDomainParticipantListener,
                PyDomainParticipantListener,
                PyNoOpDomainParticipantListenerTrampoline<>>(
                m,
                "NoOpDomainParticipantListener");
    });
}

}  // namespace pyrti
