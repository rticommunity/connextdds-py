#include "PyConnext.hpp"
#include "PyEntity.hpp"
#include <dds/core/Entity.hpp>

using namespace dds::core;

template<>
void pyrti::init_class_defs(py::class_<pyrti::PyEntity, pyrti::PyIEntity>& cls) {}

template<>
void pyrti::init_class_defs(py::class_<pyrti::PyIEntity>& cls) {
    cls
        .def(
            "enable",
            &PyIEntity::py_enable,
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
        .def(
            "__eq__",
            [](pyrti::PyIEntity& e, pyrti::PyIEntity& other) {
                return e.get_entity() == other.get_entity();
            },
            py::is_operator(),
            "Test for equality."
        )
        .def(
            "__ne__",
            [](pyrti::PyIEntity& e, pyrti::PyIEntity& other) {
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
void pyrti::process_inits<Entity>(py::module& m, pyrti::ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return pyrti::init_class<pyrti::PyIEntity>(m, "IEntity");
        }
    );
    l.push_back(
        [m]() mutable {
            return pyrti::init_class<pyrti::PyEntity, pyrti::PyIEntity>(m, "Entity");
        }
    ); 
}