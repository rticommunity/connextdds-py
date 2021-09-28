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
#include <dds/topic/ContentFilteredTopic.hpp>
#include "PyTopic.hpp"

namespace pyrti {

template<typename T>
class PyContentFilteredTopic : public dds::topic::ContentFilteredTopic<T>,
                               public PyITopicDescription<T>,
                               public PyIAnyTopic {
public:
    using dds::topic::ContentFilteredTopic<T>::ContentFilteredTopic;

    PyContentFilteredTopic(PyTopic<T>& t, std::string& n, dds::topic::Filter& f)
            : dds::topic::ContentFilteredTopic<T>(t, n, f)
    {
    }

    dds::topic::TopicDescription<T> get_topic_description() override
    {
        auto td = dds::topic::TopicDescription<T>(*this);
        return td;
    }

    const std::string py_name() const override
    {
        return this->name();
    }

    const std::string py_type_name() const override
    {
        return this->type_name();
    }

    const PyDomainParticipant py_participant() const override
    {
        dds::domain::DomainParticipant dp(this->participant());
        return PyDomainParticipant(dp);
    }

    dds::core::Entity get_entity() override
    {
        return dds::core::Entity(*this);
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

    void py_close() override
    {
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

    dds::topic::AnyTopic get_any_topic() const override
    {
        return dds::topic::AnyTopic(this->topic());
    }

    PyDomainParticipant py_domain_participant() const override
    {
        return this->py_participant();
    }

    dds::core::status::InconsistentTopicStatus py_inconsistent_topic_status()
            override
    {
        return this->topic().inconsistent_topic_status();
    }

    dds::topic::qos::TopicQos py_qos() const override
    {
        return this->topic().qos();
    }

    void py_qos(const dds::topic::qos::TopicQos& q) override
    {
        auto t = dds::topic::Topic<T>(this->topic());
        t.qos(q);
    }
};

template<typename T>
void init_content_filtered_topics_defs(
        py::class_<
            PyContentFilteredTopic<T>,
            PyITopicDescription<T>,
            PyIAnyTopic,
            std::unique_ptr<PyContentFilteredTopic<T>, no_gil_delete<PyContentFilteredTopic<T>>>>& cls)
{
    cls.def(py::init<PyTopic<T>&, std::string&, dds::topic::Filter&>(),
            py::arg("topic"),
            py::arg("name"),
            py::arg("contentfilter"),
            py::call_guard<py::gil_scoped_release>(),
            "Create a ContentFilteredTopic with a name and Filter.")
            .def(py::init([](PyITopicDescription<T>& td) {
                     auto desc = td.get_topic_description();
                     return dds::core::polymorphic_cast<
                             PyContentFilteredTopic<T>>(desc);
                 }),
                 py::arg("topic_description"),
                 py::call_guard<py::gil_scoped_release>(),
                 "Cast a TopicDescription to a ContentFilteredTopic.")
            .def(py::init([](PyIEntity& e) {
                     auto entity = e.get_entity();
                     return dds::core::polymorphic_cast<
                             PyContentFilteredTopic<T>>(entity);
                 }),
                 py::arg("entity"),
                 py::call_guard<py::gil_scoped_release>(),
                 "Cast an Entity to a ContentFilteredTopic.")
            .def_property_readonly(
                    "topic",
                    [](PyContentFilteredTopic<T>& cft) {
                        py::gil_scoped_release release;
                        auto t = cft.topic();
                        return PyTopic<T>(t);
                    },
                    "Get the underlying Topic.")
            .def_property_readonly(
                    "filter_expression",
                    [](PyContentFilteredTopic<T>& cft) {
                        py::gil_scoped_release release;
                        return cft.filter_expression();
                    },
                    "Get the filter expression")
            .def_property(
                    "filter_parameters",
                    [](PyContentFilteredTopic<T>& cft) {
                        py::gil_scoped_release release;
                        return cft.filter_parameters();
                    },
                    [](PyContentFilteredTopic<T>& cft,
                       std::vector<std::string>& params) {
                        py::gil_scoped_release release;
                        cft.filter_parameters(params.begin(), params.end());
                    },
                    "Get/set the filter parameters.")
            .def(
                    "set_filter",
                    [](PyContentFilteredTopic<T>& cft,
                       const dds::topic::Filter& filter) {
                        cft->filter(filter);
                    },
                    py::call_guard<py::gil_scoped_release>(),
                    "Set the filter.")
            .def(
                    "append_to_expression_parameter",
                    [](PyContentFilteredTopic<T>& cft,
                       int32_t index,
                       const std::string& extension) {
                        cft->append_to_expression_parameter(index, extension);
                    },
                    py::arg("index"),
                    py::arg("extension"),
                    py::call_guard<py::gil_scoped_release>(),
                    "Append the extension to the end of parameter at the "
                    "provided index, separated by a comma.")
            .def(
                    "remove_from_expression_parameter",
                    [](PyContentFilteredTopic<T>& cft,
                       int32_t index,
                       const std::string& remove_term) {
                        cft->remove_from_expression_parameter(
                                index,
                                remove_term);
                    },
                    py::arg("index"),
                    py::arg("remove_term"),
                    py::call_guard<py::gil_scoped_release>(),
                    "Removes the specified term from the parameter at the "
                    "provided index.")
            .def_static(
                    "find",
                    [](PyDomainParticipant& dp, const std::string& name) {
                        dds::core::optional<PyContentFilteredTopic<T>> retval;
                        auto t = dds::topic::find<dds::topic::ContentFilteredTopic<T>>(
                                dp,
                                name);
                        if (t != dds::core::null)
                            retval = PyContentFilteredTopic<T>(t);
                        return retval;
                    },
                    py::call_guard<py::gil_scoped_release>(),
                    py::arg("participant"),
                    py::arg("name"),
                    "Look up a ContentFilteredTopic by its name in the "
                    "DomainParticipant.")
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.");

    py::implicitly_convertible<
            PyITopicDescription<T>,
            PyContentFilteredTopic<T>>();
    py::implicitly_convertible<PyIEntity, PyContentFilteredTopic<T>>();
}

template<typename T>
void init_content_filtered_topic(py::class_<
                                 PyContentFilteredTopic<T>,
                                 PyITopicDescription<T>,
                                 PyIAnyTopic,
                                 std::unique_ptr<PyContentFilteredTopic<T>, no_gil_delete<PyContentFilteredTopic<T>>>>& cft)
{
    init_content_filtered_topics_defs(cft);
}

}  // namespace pyrti
