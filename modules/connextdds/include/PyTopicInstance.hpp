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
#include <dds/topic/TopicInstance.hpp>

namespace dds { namespace topic {
template<typename T>
std::ostream& operator<<(
        std::ostream& out,
        const dds::topic::TopicInstance<T>& val)
{
    return (out << val.handle());
}

template<typename T>
bool operator==(
        const dds::topic::TopicInstance<T>& left,
        const dds::topic::TopicInstance<T>& right)
{
    return (left.sample() == right.sample())
            && (left.handle() == right.handle());
}
}}  // namespace dds::topic

namespace pyrti {

template<typename T>
void init_dds_typed_topic_instance_base_template(
        py::class_<dds::topic::TopicInstance<T>>& cls)
{
    cls.def(py::init<const dds::core::InstanceHandle&, const T&>(),
            py::arg("handle"),
            py::arg("sample"),
            "Create a TopicInstance with an InstanceHandle and a data sample.")
            .def_property(
                    "handle",
                    (dds::core::InstanceHandle(
                            dds::topic::TopicInstance<T>::*)() const)
                            & dds::topic::TopicInstance<T>::handle,
                    (void (dds::topic::TopicInstance<T>::*)(
                            const dds::core::InstanceHandle&))
                            & dds::topic::TopicInstance<T>::handle,
                    "Get/set the instance handle associated with this "
                    "TopicInstance.")
            .def_property(
                    "sample",
                    (T & (dds::topic::TopicInstance<T>::*) ())
                            & dds::topic::TopicInstance<T>::sample,
                    (void (dds::topic::TopicInstance<T>::*)(const T&))
                            & dds::topic::TopicInstance<T>::sample,
                    "Get/set the sample associated with this TopicInstance.");
}

template<typename T>
void init_dds_typed_topic_instance_template(
        py::class_<dds::topic::TopicInstance<T>>& cls)
{
    init_dds_typed_topic_instance_base_template<T>(cls);
    cls.def(py::init<>(), "Create a null TopicInstance.")
            .def(py::init<const dds::core::InstanceHandle&>(),
                 py::arg("handle"),
                 "Create a TopicInstance with an InstanceHandle.");
}

template<typename T>
void init_topic_instance(py::class_<dds::topic::TopicInstance<T>>& ti)
{
    init_dds_typed_topic_instance_template(ti);
}

}  // namespace pyrti
