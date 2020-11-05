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

#pragma once

#include "PyConnext.hpp"
#include <pybind11/stl_bind.h>
#include <pybind11/operators.h>
#include <pybind11/functional.h>
#include <dds/sub/DataReader.hpp>
#include <rti/sub/AckResponseData.hpp>
#include <dds/sub/cond/ReadCondition.hpp>
#include <dds/sub/cond/QueryCondition.hpp>
#include <dds/sub/Query.hpp>
#include <dds/sub/find.hpp>
#include "PyEntity.hpp"
#include "PyCondition.hpp"
#include "PyAnyDataReader.hpp"
#include "PyTopic.hpp"
#include "PyDataReaderListener.hpp"
#include "PyContentFilteredTopic.hpp"
#include "PyDynamicTypeMap.hpp"
#include "PyAsyncioExecutor.hpp"

namespace pyrti {

template<typename T>
class PyDataReaderListener;

class PyIDataReader : public PyIAnyDataReader, public PyIEntity {
public:
    virtual dds::sub::Query create_query(const std::string&) = 0;

    virtual dds::sub::Query create_query(
            const std::string&,
            const std::vector<std::string>&) = 0;
};

template<typename T>
class PyDataReader : public dds::sub::DataReader<T>, public PyIDataReader {
public:
    using dds::sub::DataReader<T>::DataReader;

    PyDataReader(
            const PySubscriber& s,
            const PyTopic<T>& t,
            const dds::sub::qos::DataReaderQos& q,
            PyDataReaderListener<T>* l,
            const dds::core::status::StatusMask& m)
            : dds::sub::DataReader<T>(s, t, q, l, m)
    {
        if (nullptr != l)
            py::cast(l).inc_ref();
    }

    PyDataReader(
            const PySubscriber& s,
            const PyContentFilteredTopic<T>& t,
            const dds::sub::qos::DataReaderQos& q,
            PyDataReaderListener<T>* l,
            const dds::core::status::StatusMask& m)
            : dds::sub::DataReader<T>(s, t, q, l, m)
    {
        if (nullptr != l)
            py::cast(l).inc_ref();
    }

    virtual ~PyDataReader()
    {
        if (*this != dds::core::null) {
            if (this->delegate().use_count() <= 2 && !this->delegate()->closed()
                && nullptr != this->listener()) {
                py::object listener = py::cast(this->listener());
                this->listener(nullptr, dds::core::status::StatusMask::none());
                listener.dec_ref();
            }
        }
    }

    dds::core::Entity get_entity() override
    {
        return dds::core::Entity(*this);
    }

    dds::sub::AnyDataReader get_any_datareader() const override
    {
        return dds::sub::AnyDataReader(*this);
    }

    void py_enable() override
    {
        this->enable();
    }

    const dds::core::status::StatusMask py_status_changes() override
    {
        return this->status_changes();
    }

    const dds::core::InstanceHandle py_instance_handle() const override
    {
        return this->instance_handle();
    }

    dds::sub::qos::DataReaderQos py_qos() const override
    {
        return this->qos();
    }

    void py_qos(const dds::sub::qos::DataReaderQos& q) override
    {
        return this->qos(q);
    }

    const std::string& py_topic_name() const override
    {
        return this->delegate()->topic_name();
    }

    const std::string& py_type_name() const override
    {
        return this->delegate()->type_name();
    }

    const PySubscriber py_subscriber() const override
    {
        auto s = this->subscriber();
        return PySubscriber(s);
    }

    void py_close() override
    {
        if (nullptr != this->listener()) {
            py::object listener = py::cast(this->listener());
            this->listener(nullptr, dds::core::status::StatusMask::none());
            listener.dec_ref();
        }
        this->close();
    }

    void py_retain() override
    {
        this->retain();
    }

    bool py_closed() override
    {
        return this->delegate()->closed();
    }

    bool py_enabled() override
    {
        return this->delegate()->enabled();
    }

    int py_use_count() override
    {
        return this->delegate().use_count();
    }

    void py_unretain() override
    {
        this->delegate()->unretain();
    }

    dds::sub::Query create_query(const std::string& expression) override
    {
        return dds::sub::Query(*this, expression);
    }

    dds::sub::Query create_query(
            const std::string& expression,
            const std::vector<std::string>& params) override
    {
        return dds::sub::Query(*this, expression, params);
    }
};

template<typename T>
void init_dds_typed_datareader_base_template(
        py::class_<PyDataReader<T>, PyIDataReader>& cls)
{
    py::class_<typename PyDataReader<T>::Selector> selector(cls, "Selector");

    cls.def(py::init<const PySubscriber&, const PyTopic<T>&>(),
            py::arg("sub"),
            py::arg("topic"),
            py::call_guard<py::gil_scoped_release>(),
            "Create a DataReader.")
            .def(py::init([](const PySubscriber& s,
                             const PyTopic<T>& t,
                             const dds::sub::qos::DataReaderQos& q,
                             dds::core::optional<PyDataReaderListener<T>*> l,
                             const dds::core::status::StatusMask& m) {
                     auto listener = has_value(l) ? get_value(l) : nullptr;
                     return PyDataReader<T>(s, t, q, listener, m);
                 }),
                 py::arg("sub"),
                 py::arg("topic"),
                 py::arg("qos"),
                 py::arg("listener") = py::none(),
                 py::arg_v(
                         "mask",
                         dds::core::status::StatusMask::all(),
                         "StatusMask.all()"),
                 py::call_guard<py::gil_scoped_release>(),
                 "Create a DataReader.")
            .def(py::init<
                         const PySubscriber&,
                         const PyContentFilteredTopic<T>&>(),
                 py::arg("sub"),
                 py::arg("cft"),
                 py::call_guard<py::gil_scoped_release>(),
                 "Create a DataReader with a ContentFilteredTopic.")
            .def(py::init([](const PySubscriber& s,
                             const PyContentFilteredTopic<T>& t,
                             const dds::sub::qos::DataReaderQos& q,
                             dds::core::optional<PyDataReaderListener<T>*> l,
                             const dds::core::status::StatusMask& m) {
                     auto listener = l.is_set() ? l.get() : nullptr;
                     return PyDataReader<T>(s, t, q, listener, m);
                 }),
                 py::arg("sub"),
                 py::arg("cft"),
                 py::arg("qos"),
                 py::arg("listener") = py::none(),
                 py::arg_v(
                         "mask",
                         dds::core::status::StatusMask::all(),
                         "StatusMask.all()"),
                 py::call_guard<py::gil_scoped_release>(),
                 "Create a DataReader with a ContentFilteredTopic.")
            .def(py::init([](PyIAnyDataReader& adr) {
                     auto dr = adr.get_any_datareader().get<T>();
                     return PyDataReader<T>(dr);
                 }),
                 py::arg("reader"),
                 py::call_guard<py::gil_scoped_release>(),
                 "Create a typed DataReader from an AnyDataReader.")
            .def(py::init([](PyIEntity& e) {
                     auto entity = e.get_entity();
                     return dds::core::polymorphic_cast<PyDataReader<T>>(
                             entity);
                 }),
                 py::arg("entity"),
                 py::call_guard<py::gil_scoped_release>(),
                 "Create a typed DataReader from an Entity.")
            .def_property(
                    "default_filter_state",
                    (const dds::sub::status::DataState& (PyDataReader<T>::*) ())
                            & PyDataReader<T>::default_filter_state,
                    [](PyDataReader<T>& dr,
                       const dds::sub::status::DataState& ds) {
                        dr.default_filter_state(ds);
                        return dr;
                    },
                    py::call_guard<py::gil_scoped_release>(),
                    "Returns the filter state for the read/take operations.")
            .def(
                    py::self == py::self,
                    py::call_guard<py::gil_scoped_release>(),
                    "Test for equality.")
            .def(
                    py::self != py::self,
                    py::call_guard<py::gil_scoped_release>(),
                    "Test for inequality.")
            .def("read",
                 (dds::sub::LoanedSamples<T>(PyDataReader<T>::*)())
                         & PyDataReader<T>::read,
                 py::call_guard<py::gil_scoped_release>(),
                 "Read all samples using the default filter state")
            .def("take",
                 (dds::sub::LoanedSamples<T>(PyDataReader<T>::*)())
                         & PyDataReader<T>::take,
                 py::call_guard<py::gil_scoped_release>(),
                 "Take all samples using the default filter state")
#if rti_connext_version_gte(6, 0, 0)
            .def(
                    "read_valid",
                    [](PyDataReader<T>& dr) {
                        return rti::sub::ValidLoanedSamples<T>(dr.read());
                        ;
                    },
                    py::call_guard<py::gil_scoped_release>(),
                    "Read only valid samples.")
            .def(
                    "take_valid",
                    [](PyDataReader<T>& dr) {
                        return rti::sub::ValidLoanedSamples<T>(dr.take());
                        ;
                    },
                    py::call_guard<py::gil_scoped_release>(),
                    "Take only valid samples.")
#endif
            .def(
                    "select",
                    [](PyDataReader<T>& dr) {
                        return typename PyDataReader<T>::Selector(dr.select());
                    },
                    py::call_guard<py::gil_scoped_release>(),
                    "Get a Selector to perform complex data selections, such "
                    "as "
                    "per-instance selection, content, and status filtering.")
            .def_property_readonly(
                    "topic_description",
                    [](const PyDataReader<T>& dr) {
                        return PyTopicDescription<T>(dr.topic_description());
                    },
                    py::call_guard<py::gil_scoped_release>(),
                    "Returns the TopicDescription associated with the "
                    "DataReader.")
            .def_property_readonly(
                    "subscriber",
                    &PyDataReader<T>::py_subscriber,
                    py::call_guard<py::gil_scoped_release>(),
                    "Returns the parent Subscriber of the DataReader.")
            .def_property_readonly(
                    "listener",
                    [](PyDataReader<T>& dr) {
                        dds::core::optional<PyDataReaderListener<T>*> l;
                        auto ptr = dynamic_cast<PyDataReaderListener<T>*>(
                                dr.listener());
                        if (nullptr != ptr)
                            l = ptr;
                        return l;
                    },
                    py::call_guard<py::gil_scoped_release>(),
                    "Get the listener object.")
            .def(
                    "bind_listener",
                    [](PyDataReader<T>& dr,
                       dds::core::optional<PyDataReaderListener<T>*> l,
                       const dds::core::status::StatusMask& m) {
                        auto listener = has_value(l) ? get_value(l) : nullptr;
                        if (nullptr != listener) {
                            py::object py_l = py::cast(listener);
                            py_l.inc_ref();
                        }
                        if (nullptr != dr.listener()) {
                            py::cast(dr.listener()).dec_ref();
                        }
                        dr.listener(listener, m);
                    },
                    py::arg("listener"),
                    py::arg("event_mask"),
                    py::call_guard<py::gil_scoped_release>(),
                    "Set the listener and associated event mask.")
            .def_property(
                    "qos",
                    (dds::sub::qos::DataReaderQos(PyDataReader<T>::*)() const)
                            & PyDataReader<T>::qos,
                    (void (PyDataReader<T>::*)(
                            const dds::sub::qos::DataReaderQos&))
                            & PyDataReader<T>::qos,
                    py::call_guard<py::gil_scoped_release>(),
                    "The DataReaderQos for this DataReader."
                    "\n\n"
                    "This property's getter returns a deep copy.")
            .def(
                    "__lshift__",
                    [](PyDataReader<T>& dr,
                       const dds::sub::qos::DataReaderQos& qos) {
                        dr << qos;
                        return dr;
                    },
                    py::is_operator(),
                    py::call_guard<py::gil_scoped_release>(),
                    "Set the DataReaderQos for this DataReader.")
            .def(
                    "__rshift__",
                    [](PyDataReader<T>& dr, dds::sub::qos::DataReaderQos& qos) {
                        dr >> qos;
                        return dr;
                    },
                    py::is_operator(),
                    py::call_guard<py::gil_scoped_release>(),
                    "Get the DataReaderQos from this DataReader"

                    )
            .def("wait_for_historical_data",
                 &PyDataReader<T>::wait_for_historical_data,
                 py::arg("max_wait"),
                 py::call_guard<py::gil_scoped_release>(),
                 "Waits until all \"historical\" data is received for "
                 "DataReaders "
                 "that have a non-VOLATILE Durability Qos kind.")
            .def(
                    "wait_for_historical_data_async",
                    [](PyDataReader<T>& dr,
                       const dds::core::Duration& max_wait) {
                        return PyAsyncioExecutor::run<void>(
                                std::function<void()>([&dr, &max_wait]() {
                                    dr.wait_for_historical_data(max_wait);
                                }));
                    },
                    py::arg("max_wait"),
                    py::keep_alive<0, 1>(),
                    py::keep_alive<0, 2>(),
                    "Waits until all \"historical\" data is received for "
                    "DataReaders "
                    "that have a non-VOLATILE Durability Qos kind. This call "
                    "is "
                    "awaitable and only for use with asyncio.")
            .def_property_readonly(
                    "liveliness_changed_status",
                    &PyDataReader<T>::liveliness_changed_status,
                    py::call_guard<py::gil_scoped_release>(),
                    "Get the LivelinessChangedStatus for this DataReader.")
            .def_property_readonly(
                    "sample_rejected_status",
                    &PyDataReader<T>::sample_rejected_status,
                    py::call_guard<py::gil_scoped_release>(),
                    "Get the SampleRejectedStatus for the DataReader.")
            .def_property_readonly(
                    "sample_lost_status",
                    &PyDataReader<T>::sample_lost_status,
                    py::call_guard<py::gil_scoped_release>(),
                    "Get the SampleLostStatus for the DataReader.")
            .def_property_readonly(
                    "requested_deadline_missed_status",
                    &PyDataReader<T>::requested_deadline_missed_status,
                    py::call_guard<py::gil_scoped_release>(),
                    "Get the RequestedDeadlineMissed status for the DataReader")
            .def_property_readonly(
                    "requested_incompatible_qos_status",
                    &PyDataReader<T>::requested_incompatible_qos_status,
                    py::call_guard<py::gil_scoped_release>(),
                    "Get the RequestedIncompatibleQosStatus for the "
                    "DataReader.")
            .def_property_readonly(
                    "subscription_matched_status",
                    &PyDataReader<T>::subscription_matched_status,
                    py::call_guard<py::gil_scoped_release>(),
                    "Get the SubscriptionMatchedStatus for the DataReader.")
            .def_property_readonly(
                    "datareader_cache_status",
                    [](PyDataReader<T>& dr) {
                        return dr->datareader_cache_status();
                    },
                    py::call_guard<py::gil_scoped_release>(),
                    "Get the DataReaderCacheStatus for the DataReader.")
            .def_property_readonly(
                    "datareader_protocol_status",
                    [](PyDataReader<T>& dr) {
                        return dr->datareader_protocol_status();
                    },
                    py::call_guard<py::gil_scoped_release>(),
                    "Get the DataReaderProtocolStatus for the DataReader.")
            .def(
                    "matched_publication_datareader_protocol_status",
                    [](PyDataReader<T>& dr,
                       const dds::core::InstanceHandle& handle) {
                        return dr
                                ->matched_publication_datareader_protocol_status(
                                        handle);
                    },
                    py::arg("publication_handle"),
                    py::call_guard<py::gil_scoped_release>(),
                    "Get the DataReaderProtocolStatus for the DataReader based "
                    "on the "
                    "matched publication handle.")
            .def(
                    "acknowledge_all",
                    [](PyDataReader<T>& dr) { dr->acknowledge_all(); },
                    py::call_guard<py::gil_scoped_release>(),
                    "Acknowledge all previously accessed samples.")
            .def(
                    "acknowledge_all",
                    [](PyDataReader<T>& dr,
                       const rti::sub::AckResponseData& rd) {
                        dr->acknowledge_all(rd);
                    },
                    py::call_guard<py::gil_scoped_release>(),
                    "Acknowledge all previously accessed samples.")
            .def(
                    "acknowledge_sample",
                    [](PyDataReader<T>& dr, const dds::sub::SampleInfo& info) {
                        dr->acknowledge_sample(info);
                    },
                    py::arg("sample_info"),
                    py::call_guard<py::gil_scoped_release>(),
                    "Acknowledge a single sample.")
#if rti_connext_version_gte(6, 0, 0)
            .def(
                    "is_data_consistent",
                    [](PyDataReader<T>& dr,
                       const T& data,
                       const dds::sub::SampleInfo& info) {
                        return dr->is_data_consistent(data, info);
                    },
                    py::arg("sample_data"),
                    py::arg("sample_info"),
                    py::call_guard<py::gil_scoped_release>(),
                    "Checks if the sample has been overwritten by the "
                    "DataWriter.")
            .def(
                    "is_data_consistent",
                    [](PyDataReader<T>& dr, const dds::sub::Sample<T>& sample) {
                        return dr->is_data_consistent(
                                sample.data(),
                                sample.info());
                    },
                    py::arg("sample"),
                    py::call_guard<py::gil_scoped_release>(),
                    "Checks if the sample has been overwritten by the "
                    "DataWriter.")
            .def(
                    "is_data_consistent",
                    [](PyDataReader<T>& dr,
                       const rti::sub::LoanedSample<T>& sample) {
                        return dr->is_data_consistent(sample);
                    },
                    py::arg("sample"),
                    py::call_guard<py::gil_scoped_release>(),
                    "Checks if the sample has been overwritten by the "
                    "DataWriter.")
#endif
            .def_property_readonly(
                    "topic_name",
                    [](PyDataReader<T>& dr) { return dr.py_topic_name(); },
                    py::call_guard<py::gil_scoped_release>(),
                    "Get the topic name associated with this DataReader.")
            .def_property_readonly(
                    "type_name",
                    [](PyDataReader<T>& dr) { return dr.py_type_name(); },
                    py::call_guard<py::gil_scoped_release>(),
                    "Get the type name associated with this DataReader.")
            .def(
                    "close",
                    [](PyDataReader<T>& dr) { dr->close(); },
                    py::call_guard<py::gil_scoped_release>(),
                    "Close this DataReader.")
            .def(
                    "__enter__",
                    [](PyDataReader<T>& dr) { return dr; },
                    py::call_guard<py::gil_scoped_release>(),
                    "Enter a context for this DataReader, to be cleaned up on "
                    "exiting context")
            .def(
                    "__exit__",
                    [](PyDataReader<T>& dr,
                       py::object,
                       py::object,
                       py::object) { dr->close(); },
                    py::call_guard<py::gil_scoped_release>(),
                    "Exit the context for this DataReader, cleaning up "
                    "resources.")
            .def_static(
                    "find_all_by_topic",
                    [](PySubscriber& sub, const std::string& topic_name) {
                        std::vector<PyDataReader<T>> v;
                        dds::sub::find<PyDataReader<T>>(
                                sub,
                                topic_name,
                                std::back_inserter(v));
                        return v;
                    },
                    py::arg("subscriber"),
                    py::arg("topic_name"),
                    py::call_guard<py::gil_scoped_release>(),
                    "Retrieve all DataReaders for the given topic name in the "
                    "subscriber.")
            .def_static(
                    "find_by_name",
                    [](PyDomainParticipant& dp, const std::string name) {
                        dds::core::optional<PyDataReader<T>> retval;
                        auto dr = rti::sub::find_datareader_by_name<
                                PyDataReader<T>>(dp, name);
                        if (dr != dds::core::null)
                            retval = dr;
                        return retval;
                    },
                    py::arg("participant"),
                    py::arg("name"),
                    py::call_guard<py::gil_scoped_release>(),
                    "Find DataReader in DomainParticipant with the "
                    "DataReader's "
                    "name, returning the first found.")
            .def_static(
                    "find_by_name",
                    [](PySubscriber& sub, const std::string& name) {
                        dds::core::optional<PyDataReader<T>> retval;
                        auto dr = rti::sub::find_datareader_by_name<
                                PyDataReader<T>>(sub, name);
                        if (dr != dds::core::null)
                            retval = dr;
                        return retval;
                    },
                    py::arg("subscriber"),
                    py::arg("name"),
                    py::call_guard<py::gil_scoped_release>(),
                    "Find DataReader in Subscriber with the DataReader's name, "
                    "returning the first found.")
            .def_static(
                    "find_by_topic",
                    [](PySubscriber& sub, const std::string& topic_name) {
                        dds::core::optional<PyDataReader<T>> retval;
                        auto dr = rti::sub::find_datareader_by_topic_name<
                                PyDataReader<T>>(sub, topic_name);
                        if (dr != dds::core::null)
                            retval = dr;
                        return retval;
                    },
                    py::arg("subscriber"),
                    py::arg("name"),
                    py::call_guard<py::gil_scoped_release>(),
                    "Find DataReader in Subscriber with a topic name, "
                    "returning the first found.");

    selector.def(py::init<PyDataReader<T>&>(),
                 py::arg("datareader"),
                 py::call_guard<py::gil_scoped_release>(),
                 "Create a Selector for a DataReader to read/take based on "
                 "selected conditions")
            .def("instance",
                 &PyDataReader<T>::Selector::instance,
                 py::arg("handle"),
                 py::call_guard<py::gil_scoped_release>(),
                 "Select a specific instance to read/take.")
            .def("next_instance",
                 &PyDataReader<T>::Selector::next_instance,
                 py::arg("previous"),
                 py::call_guard<py::gil_scoped_release>(),
                 "Select the instance after the specified instance to "
                 "read/take.")
            .def("state",
                 &PyDataReader<T>::Selector::state,
                 py::arg("state"),
                 py::call_guard<py::gil_scoped_release>(),
                 "Select samples with a specified data state.")
            .def("content",
                 &PyDataReader<T>::Selector::content,
                 py::arg("query"),
                 py::call_guard<py::gil_scoped_release>(),
                 "Select samples based on a Query.")
            .def(
                    "condition",
                    [](typename PyDataReader<T>::Selector& sel,
                       PyIReadCondition& rc) {
                        auto cond = rc.get_read_condition();
                        return sel.condition(cond);
                    },
                    py::arg("condition"),
                    py::call_guard<py::gil_scoped_release>(),
                    "Select samples based on a ReadCondition.")
            .def("max_samples",
                 &PyDataReader<T>::Selector::max_samples,
                 py::arg("max"),
                 py::call_guard<py::gil_scoped_release>(),
                 "Limit the number of samples read/taken by the Select.")
            .def("read",
                 (dds::sub::LoanedSamples<T>(PyDataReader<T>::Selector::*)())
                         & PyDataReader<T>::Selector::read,
                 py::call_guard<py::gil_scoped_release>(),
                 "Read samples based on Selector settings.")
#if rti_connext_version_gte(6, 0, 0)
            .def(
                    "read_valid",
                    [](typename PyDataReader<T>::Selector& s) {
                        return rti::sub::ValidLoanedSamples<T>(s.read());
                    },
                    py::call_guard<py::gil_scoped_release>(),
                    "Read valid samples based on Selector settings.")
#endif
            .def("take",
                 (dds::sub::LoanedSamples<T>(PyDataReader<T>::Selector::*)())
                         & PyDataReader<T>::Selector::take,
                 py::call_guard<py::gil_scoped_release>(),
                 "Take samples based on Selector settings.")
#if rti_connext_version_gte(6, 0, 0)
            .def(
                    "take_valid",
                    [](typename PyDataReader<T>::Selector& s) {
                        return rti::sub::ValidLoanedSamples<T>(s.take());
                    },
                    py::call_guard<py::gil_scoped_release>(),
                    "Take valid samples based on Selector settings.")
#endif
            ;

    py::implicitly_convertible<PyIAnyDataReader, PyDataReader<T>>();
    py::implicitly_convertible<PyIEntity, PyDataReader<T>>();
}

template<typename T>
void init_dds_typed_datareader_template(
        py::class_<PyDataReader<T>, PyIDataReader>& cls)
{
    init_dds_typed_datareader_base_template<T>(cls);

    cls.def(
               "key_value",
               [](PyDataReader<T>& dr,
                  const dds::core::InstanceHandle& handle) {
                   T d;
                   dr.key_value(d, handle);
                   return d;
               },
               py::arg("handle"),
               py::call_guard<py::gil_scoped_release>(),
               "Retrieve the instance key that corresponds to an instance "
               "handle.")
            .def(
                    "topic_instance_key_value",
                    [](PyDataReader<T>& dr,
                       const dds::core::InstanceHandle& handle) {
                        T d;
                        dds::topic::TopicInstance<T> ti(handle, d);
                        dr.key_value(ti, handle);
                        return ti;
                    },
                    py::arg("handle"),
                    py::call_guard<py::gil_scoped_release>(),
                    "Retrieve the instance key that corresponds to an instance "
                    "handle.")
            .def(
                    "read_next",
                    [](PyDataReader<T>& dr) {
                        dds::core::optional<dds::sub::Sample<T>> retval;
                        T data;
                        dds::sub::SampleInfo info;
                        if (dr->read(data, info)) {
                            retval = dds::sub::Sample<T>(data, info);
                        }
                        return retval;
                    },
                    py::call_guard<py::gil_scoped_release>(),
                    "Copy the next not-previously-accessed data value from the "
                    "DataReader via a read operation.")
            .def(
                    "take_next",
                    [](PyDataReader<T>& dr) {
                        dds::core::optional<dds::sub::Sample<T>> retval;
                        T data;
                        dds::sub::SampleInfo info;
                        if (dr->take(data, info)) {
                            retval = dds::sub::Sample<T>(data, info);
                        }
                        return retval;
                    },
                    py::call_guard<py::gil_scoped_release>(),
                    "Copy the next not-previously-accessed data value from the "
                    "DataReader via a take operation.");
}

template<typename T>
void init_datareader(py::class_<PyDataReader<T>, PyIDataReader>& dr)
{
    init_dds_typed_datareader_template(dr);
}

}  // namespace pyrti