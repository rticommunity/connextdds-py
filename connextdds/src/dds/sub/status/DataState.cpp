#include "PyConnext.hpp"
#include <dds/sub/status/DataState.hpp>
#include "PyMaskType.hpp"

using namespace dds::sub::status;

template<>
void pyrti::init_class_defs(py::class_<SampleState>& cls) {
    cls
        .def_static(
            "read",
            &SampleState::read,
            "Creates a read SampleState."
        )
        .def_static(
            "not_read",
            &SampleState::not_read,
            "Creates a not_read SampleState object."
        )
        .def_static(
            "any",
            &SampleState::any,
            "Creates a SampleState object representing any sample state."
        );
}

template<>
void pyrti::init_class_defs(py::class_<ViewState>& cls) {
    cls
        .def_static(
            "new_view",
            &ViewState::new_view,
            "Creates a new_view ViewState object."
        )
        .def_static(
            "not_new_view",
            &ViewState::not_new_view,
            "Creates a not_new_view ViewState object."
        )
        .def_static(
            "any",
            &ViewState::any,
            "Creates an any ViewState object."
        );
}

template<>
void pyrti::init_class_defs(py::class_<InstanceState>& cls) {
    cls
        .def_static(
            "alive",
            &InstanceState::alive,
            "Creates a alive InstanceState object."
        )
        .def_static(
            "not_alive_disposed",
            &InstanceState::not_alive_disposed,
            "Creates a not_alive_disposed InstanceState object."
        )
        .def_static(
            "not_alive_no_writers",
            &InstanceState::not_alive_no_writers,
            "Creates a not_alive_no_writers InstanceState object."
        )
        .def_static(
            "not_alive_mask",
            &InstanceState::not_alive_mask,
            "Creates an InstanceState object representing any not alive "
            "states (not_alive_disposed | not_alive_no_writers"
        )
        .def_static(
            "any",
            &InstanceState::any,
            "Creates an InstanceState object representing any instance states "
            "(not_alive_disposed | not_alive_no_writers | alive"
        );
}

template<>
void pyrti::init_class_defs(py::class_<DataState>& cls) {
    cls
        .def(
            py::init<>(),
            "Create a DataState with InstanceState.any(), ViewState.any(), "
            "and SampleState.any()"
        )
        .def(
            py::init<
                const SampleState&
            >(),
            py::arg("sample_state"),
            "Create a DataState with InstanceState.any(), ViewState.any(), "
            "and the provided SampleState."
        )
        .def(
            py::init<
                const ViewState&
            >(),
            py::arg("view_state"),
            "Create a DataState with InstanceState.any(), SampleState.any(), "
            "and the provided ViewState."
        )
        .def(
            py::init<
                const InstanceState&
            >(),
            py::arg("instance_state"),
            "Create a DataState with ViewState.any(), SampleState.any(), "
            "and the provided InstanceState."
        )
        .def(
            py::init<
                const SampleState&,
                const ViewState&,
                const InstanceState&
            >(),
            py::arg("sample_state"),
            py::arg("view_state"),
            py::arg("instance_state"),
            "Create a DataState with the provided SampleState, ViewState, and "
            "InstanceState."
        )
        .def(
            py::self == py::self,
            "Compare DataState objects for equality."
        )
        .def(
            py::self != py::self,
            "Compare DataState objects for inequality."
        )
        .def(
            "__lshift__",
            (DataState& (DataState::*)(const SampleState&)) &DataState::operator<<,
            py::is_operator(),
            "Set the SampleState for the DataState."
        )
        .def(
            "__lshift__",
            (DataState& (DataState::*)(const ViewState&)) &DataState::operator<<,
            py::is_operator(),
            "Set the ViewState for the DataState."
        )
        .def(
            "__lshift__",
            (DataState& (DataState::*)(const InstanceState&)) &DataState::operator<<,
            py::is_operator(),
            "Set the InstanceState for the DataState."
        )
        .def_property(
            "sample_state",
            (const SampleState& (DataState::*)() const) &DataState::sample_state,
            (void (DataState::*)(const SampleState&)) &DataState::sample_state,
            "The SampleState of the DataState."
        )
        .def_property(
            "instance_state",
            (const InstanceState& (DataState::*)() const) &DataState::instance_state,
            (void (DataState::*)(const InstanceState&)) &DataState::instance_state,
            "The InstanceState of the DataState."
        )
        .def_property(
            "view_state",
            (const ViewState& (DataState::*)() const) &DataState::view_state,
            (void (DataState::*)(const ViewState&)) &DataState::view_state,
            "The ViewState of the DataState."
        )
        .def_static(
            "any",
            &DataState::any,
            "Create a DataState with InstanceState.any(), ViewState.any(), "
            "and SampleState.any()"
        )
        .def_static(
            "new_data",
            &DataState::new_data,
            "Create a DataState with InstanceState.alive(), ViewState.any(), "
            "and SampleState.not_read()"
        )
        .def_static(
            "any_data",
            &DataState::any_data,
            "Create a DataState with InstanceState.alive(), ViewState.any(), "
            "and SampleState.any()"
        )
        .def_static(
            "new_instance",
            &DataState::new_instance,
            "Create a DataState with InstanceState.alive(), "
            "ViewState.new_view(), and SampleState.any()"
        );
}



template<>
void pyrti::process_inits<DataState>(py::module& m, pyrti::ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            auto cls = pyrti::init_mask_type<SampleState, uint32_t>(m, "SampleState", "Create a SampleState with no bits set.");
            return [cls]() mutable {
                init_class_defs<SampleState>(cls);
            };
        }
    ); 

    l.push_back(
        [m]() mutable {
            auto cls = pyrti::init_mask_type<ViewState, uint32_t>(m, "ViewState", "Create a ViewState with no bits set.");
            return [cls]() mutable {
                init_class_defs<ViewState>(cls);
            };
        }
    ); 

    l.push_back(
        [m]() mutable {
            auto cls = pyrti::init_mask_type<InstanceState, uint32_t>(m, "InstanceState", "Create an InstanceState with no bits set.");
            return [cls]() mutable {
                init_class_defs<InstanceState>(cls);
            };
        }
    ); 

    l.push_back(
        [m]() mutable {
            return pyrti::init_class<DataState>(m, "DataState");
        }
    ); 
}
