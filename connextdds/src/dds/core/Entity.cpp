#include "PyConnext.hpp"
#include "PySeq.hpp"
#include "PyEntity.hpp"
#include <dds/core/Entity.hpp>

using namespace dds::core;

namespace pyrti {

template<>
void init_class_defs(py::class_<PyEntity, PyIEntity>& cls) {}

template<>
void init_class_defs(py::class_<PyIEntity>& cls) {
    cls
        .def(
            "enable",
            &PyIEntity::py_enable,
            py::call_guard<py::gil_scoped_release>(),
            "Enables this entity (if it was created disabled)."
        )
        .def_property_readonly(
            "status_changes",
            &PyIEntity::py_status_changes,
            "The list of communication statuses that are triggered."
        )
        .def_property_readonly(
            "instance_handle",
            &PyIEntity::py_instance_handle,
            "The instance handle that represents this entity."
        )
        .def(
            "close",
            &PyIEntity::py_close,
            "Forces the destruction of this entity."
        )
        .def(
            "retain",
            &PyIEntity::py_retain,
            "Disables the automatic destruction of this entity."
        )
        .def_property_readonly(
            "closed",
            &PyIEntity::py_closed,
            "Returns a boolean indicating if this Entity is closed."
        )
        .def_property_readonly(
            "enabled",
            &PyIEntity::py_enabled,
            "Returns a boolean indicating if this Entity is enabled."
        )
        .def_property_readonly(
            "use_count",
            &PyIEntity::py_use_count,
            "Returns the internal use count value for this Entity."
        )
        .def(
            "unretain",
            &PyIEntity::py_unretain,
            "Decrement the retention count."
        )
        .def(
            "__eq__",
            [](PyIEntity& e, PyIEntity& other) {
                return e.get_entity() == other.get_entity();
            },
            py::is_operator(),
            "Test for equality."
        )
        .def(
            "__ne__",
            [](PyIEntity& e, PyIEntity& other) {
                return e.get_entity() != other.get_entity();
            },
            py::is_operator(),
            "Test for inequality."
        )
        .doc() = 
        "This is the abstract base class for all the DDS objects that support "
        "QoS policies, a listener and a status condition.";
}

template<>
void process_inits<Entity>(py::module& m, ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return init_class_with_ptr_seq<PyIEntity>(m, "IEntity");
        }
    );
    l.push_back(
        [m]() mutable {
            return init_class_with_seq<PyEntity, PyIEntity>(m, "Entity");
        }
    ); 
}

}
