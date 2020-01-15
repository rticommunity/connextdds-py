#pragma once

#include "PyConnext.hpp"
#include <pybind11/stl_bind.h>
#include <dds/topic/TopicInstance.hpp>

namespace pyrti {
    template<typename T>
    void init_dds_typed_topic_instance_base_template(py::class_<dds::topic::TopicInstance<T>>& cls) {
        cls
            .def(
                py::init<
                    const dds::core::InstanceHandle&,
                    const T&
                    >(),
                py::arg("handle"),
                py::arg("sample"),
                "Create a TopicInstance with an InstanceHandle and a data sample."
            )
            .def_property(
                "handle",
                (dds::core::InstanceHandle (dds::topic::TopicInstance<T>::*)() const) &dds::topic::TopicInstance<T>::handle,
                (void (dds::topic::TopicInstance<T>::*)(const dds::core::InstanceHandle&)) &dds::topic::TopicInstance<T>::handle,
                "Get/set the instance handle associated with this TopicInstance."
            )
            .def_property(
                "sample",
                (T& (dds::topic::TopicInstance<T>::*)()) &dds::topic::TopicInstance<T>::sample,
                (void (dds::topic::TopicInstance<T>::*)(const T&)) &dds::topic::TopicInstance<T>::sample,
                "Get/set the sample associated with this TopicInstance."
            );

        py::bind_vector<std::vector<dds::topic::TopicInstance<T>>>(cls, "Seq");
        py::bind_vector<std::vector<std::pair<dds::topic::TopicInstance<T>, dds::core::Time>>>(cls, "TimestampedSeq");

        py::implicitly_convertible<py::iterable, std::vector<dds::topic::TopicInstance<T>>>();
        py::implicitly_convertible<py::iterable, std::vector<std::pair<dds::topic::TopicInstance<T>, dds::core::Time>>>();
    }

    template<typename T>
    void init_dds_typed_topic_instance_template(py::class_<dds::topic::TopicInstance<T>>& cls) {
        init_dds_typed_topic_instance_base_template<T>(cls);
        cls
            .def(
                py::init<>(),
                "Create a null TopicInstance."
            )
            .def(
                py::init<
                    const dds::core::InstanceHandle&
                    >(),
                py::arg("handle"),
                "Create a TopicInstance with an InstanceHandle."
            );
    }

    template<typename T>
    void init_topic_instance(py::object& o) {
        py::class_<dds::topic::TopicInstance<T>> ti(o, "TopicInstance");

        init_dds_typed_topic_instance_template(ti);
    }
}