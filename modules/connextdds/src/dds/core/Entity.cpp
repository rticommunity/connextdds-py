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
#include "PySeq.hpp"
#include "PyEntity.hpp"
#include <dds/core/Entity.hpp>

using namespace dds::core;

namespace pyrti {

template<>
void init_class_defs(
        py::class_<
            PyEntity,
            PyIEntity,
            std::unique_ptr<PyEntity, no_gil_delete<PyEntity>>>&)
{
}

template<>
void init_class_defs(
        py::class_<
            PyIEntity,
            std::unique_ptr<PyIEntity, no_gil_delete<PyIEntity>>>& cls)
{
    cls.def("enable",
            &PyIEntity::py_enable,
            py::call_guard<py::gil_scoped_release>(),
            "Enables this entity (if it was created disabled).")
            .def_property_readonly(
                    "status_changes",
                    &PyIEntity::py_status_changes,
                    py::call_guard<py::gil_scoped_release>(),
                    "The list of communication statuses that are triggered.")
            .def_property_readonly(
                    "instance_handle",
                    &PyIEntity::py_instance_handle,
                    py::call_guard<py::gil_scoped_release>(),
                    "The instance handle that represents this entity.")
            .def("close",
                    &PyIEntity::py_close,
                    py::call_guard<py::gil_scoped_release>(),
                    "Forces the destruction of this entity.")
            .def("retain",
                    &PyIEntity::py_retain,
                    py::call_guard<py::gil_scoped_release>(),
                    "Disables the automatic destruction of this entity.")
            .def_property_readonly(
                    "closed",
                    &PyIEntity::py_closed,
                    "Returns a boolean indicating if this Entity is closed.")
            .def_property_readonly(
                    "enabled",
                    &PyIEntity::py_enabled,
                    py::call_guard<py::gil_scoped_release>(),
                    "Returns a boolean indicating if this Entity is enabled.")
            .def_property_readonly(
                    "use_count",
                    &PyIEntity::py_use_count,
                    py::call_guard<py::gil_scoped_release>(),
                    "Returns the internal use count value for this Entity.")
            .def("unretain",
                    &PyIEntity::py_unretain,
                    py::call_guard<py::gil_scoped_release>(),
                    "Decrement the retention count.")
            .def(
                    "__eq__",
                    [](PyIEntity& e, PyIEntity& other) {
                        return e.get_entity() == other.get_entity();
                    },
                    py::is_operator(),
                    py::call_guard<py::gil_scoped_release>(),
                    "Test for equality.")
            .def(
                    "__ne__",
                    [](PyIEntity& e, PyIEntity& other) {
                        return e.get_entity() != other.get_entity();
                    },
                    py::is_operator(),
                    py::call_guard<py::gil_scoped_release>(),
                    "Test for inequality.")
            .doc() =
            "This is the abstract base class for all the DDS objects that "
            "support "
            "QoS policies, a listener and a status condition.";
}

template<>
void process_inits<Entity>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class_with_ptr_seq<
            PyIEntity,
            std::unique_ptr<PyIEntity, no_gil_delete<PyIEntity>>>(
                m,
                "IEntity");
    });
    l.push_back([m]() mutable {
        return init_class_with_seq<
            PyEntity,
            PyIEntity,
            std::unique_ptr<PyEntity, no_gil_delete<PyEntity>>>(
                m,
                "Entity");
    });
}

}  // namespace pyrti
