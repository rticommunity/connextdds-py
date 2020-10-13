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
#include "PySubscriberListener.hpp"

namespace pyrti {

template<>
void init_class_defs(py::class_<
                     PySubscriberListener,
                     PyAnyDataReaderListener,
                     PySubscriberListenerTrampoline<>>& cls)
{
    cls.def(py::init<>())
            .def("on_data_on_readers",
                 (void (PySubscriberListener::*)(PySubscriber&))
                         & PySubscriberListener::on_data_on_readers,
                 "Data on datareaders callback.");
}

template<>
void init_class_defs(py::class_<
                     PyNoOpSubscriberListener,
                     PySubscriberListener,
                     PyNoOpSubscriberListenerTrampoline<>>& cls)
{
    cls.def(py::init<>())
            .def("on_data_on_readers",
                 (void (PyNoOpSubscriberListener::*)(PySubscriber&))
                         & PyNoOpSubscriberListener::on_data_on_readers,
                 "Data on datareaders callback.");
}

template<>
void process_inits<dds::sub::SubscriberListener>(
        py::module& m,
        ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<
                PySubscriberListener,
                PyAnyDataReaderListener,
                PySubscriberListenerTrampoline<>>(m, "SubscriberListener");
    });

    l.push_back([m]() mutable {
        return init_class<
                PyNoOpSubscriberListener,
                PySubscriberListener,
                PyNoOpSubscriberListenerTrampoline<>>(
                m,
                "NoOpSubscriberListener");
    });
}

}  // namespace pyrti
