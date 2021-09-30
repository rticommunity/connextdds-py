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
#include <dds/sub/status/DataState.hpp>
#include "PyMaskType.hpp"

using namespace dds::sub::status;

namespace pyrti {

template<>
void init_class_defs(py::class_<SampleState>& cls)
{
    cls.def(
            "__str__",
            [](const SampleState& s) {
                switch(s.to_ulong()) {
                case DDS_READ_SAMPLE_STATE:
                    return std::string("SampleState.READ");
                case DDS_NOT_READ_SAMPLE_STATE:
                    return std::string("SampleState.NOT_READ");
                case DDS_READ_SAMPLE_STATE | DDS_NOT_READ_SAMPLE_STATE:
                case 0x0000FFFF:
                    return std::string("SampleState.ANY");
                default:
                    return std::string("SampleState.INVALID");
                }
            })
            .def_property_readonly_static(
                    "READ",
                    [](py::object&) {
                        return SampleState::read();
                    },
                    "Creates a read SampleState.")
            .def_property_readonly_static(
                    "NOT_READ",
                    [](py::object&) {
                        return SampleState::not_read();
                    },
                    "Creates a not_read SampleState object.")
            .def_property_readonly_static(
                    "ANY",
                    [](py::object&) {
                        return SampleState::any();
                    },
                    "Creates a SampleState object representing any sample "
                    "state.");
}

template<>
void init_class_defs(py::class_<ViewState>& cls)
{
    cls.def(
            "__str__",
            [](const ViewState& s) {
                switch(s.to_ulong()) {
                case DDS_NEW_VIEW_STATE:
                    return std::string("ViewState.NEW");
                case DDS_NOT_NEW_VIEW_STATE:
                    return std::string("ViewState.NOT_NEW");
                case DDS_NEW_VIEW_STATE | DDS_NOT_NEW_VIEW_STATE:
                case 0x0000FFFF:
                    return std::string("ViewState.ANY");
                default:
                    return std::string("ViewState.INVALID");
                }
            })
            .def_property_readonly_static(
                    "NEW_VIEW",
                    [](py::object&) {
                        return ViewState::new_view();
                    },
                    "Creates a new_view ViewState object.")
            .def_property_readonly_static(
                    "NOT_NEW_VIEW",
                    [](py::object&) {
                        return ViewState::not_new_view();
                    },
                    "Creates a not_new_view ViewState object.")
            .def_property_readonly_static(
                    "ANY",
                    [](py::object&) {
                        return ViewState::any();
                    },
                    "Creates an any ViewState object.");
}

template<>
void init_class_defs(py::class_<InstanceState>& cls)
{
    cls.def(
            "__str__",
            [](const InstanceState& s) {
                switch(s.to_ulong()) {
                case DDS_ALIVE_INSTANCE_STATE:
                    return std::string("InstanceState.ALIVE");
                case DDS_NOT_ALIVE_DISPOSED_INSTANCE_STATE:
                    return std::string("InstanceState.NOT_ALIVE_DISPOSED");
                case DDS_NOT_ALIVE_NO_WRITERS_INSTANCE_STATE:
                    return std::string("InstanceState.NOT_ALIVE_NO_WRITERS");
                case DDS_NOT_ALIVE_DISPOSED_INSTANCE_STATE | DDS_NOT_ALIVE_NO_WRITERS_INSTANCE_STATE:
                    return std::string("InstanceState.NOT_ALIVE");
                case DDS_ALIVE_INSTANCE_STATE | DDS_NOT_ALIVE_DISPOSED_INSTANCE_STATE | DDS_NOT_ALIVE_NO_WRITERS_INSTANCE_STATE:
                case 0x0000FFFF:
                    return std::string("InstanceState.ANY");
                default:
                    return std::string("InstanceState.INVALID");
                }
            })
            .def_property_readonly_static(
                    "ALIVE",
                    [](py::object&) {
                        return InstanceState::alive();
                    },
                    "Creates a alive InstanceState object.")
            .def_property_readonly_static(
                    "NOT_ALIVE_DISPOSED",
                    [](py::object&) {
                        return InstanceState::not_alive_disposed();
                    },
                    "Creates a not_alive_disposed InstanceState object.")
            .def_property_readonly_static(
                    "NOT_ALIVE_NO_WRITERS",
                    [](py::object&) {
                        return InstanceState::not_alive_no_writers();
                    },
                    "Creates a not_alive_no_writers InstanceState object.")
            .def_property_readonly_static(
                    "NOT_ALIVE_MASK",
                    [](py::object&) {
                        return InstanceState::not_alive_mask();
                    },
                    "Creates an InstanceState object representing any not "
                    "alive "
                    "states (not_alive_disposed | not_alive_no_writers")
            .def_property_readonly_static(
                    "ANY",
                    [](py::object&) {
                        return InstanceState::any();
                    },
                    "Creates an InstanceState object representing any instance "
                    "states "
                    "(not_alive_disposed | not_alive_no_writers | alive");
}

template<>
void init_class_defs(py::class_<DataState>& cls)
{
    cls.def(py::init<>(),
            "Create a DataState with InstanceState.any(), ViewState.any(), "
            "and SampleState.ANY")
            .def(py::init<const SampleState&>(),
                 py::arg("sample_state"),
                 "Create a DataState with InstanceState.any(), "
                 "ViewState.any(), "
                 "and the provided SampleState.")
            .def(py::init<const ViewState&>(),
                 py::arg("view_state"),
                 "Create a DataState with InstanceState.any(), "
                 "SampleState.any(), "
                 "and the provided ViewState.")
            .def(py::init<const InstanceState&>(),
                 py::arg("instance_state"),
                 "Create a DataState with ViewState.any(), SampleState.any(), "
                 "and the provided InstanceState.")
            .def(py::init<
                         const SampleState&,
                         const ViewState&,
                         const InstanceState&>(),
                 py::arg("sample_state"),
                 py::arg("view_state"),
                 py::arg("instance_state"),
                 "Create a DataState with the provided SampleState, ViewState, "
                 "and "
                 "InstanceState.")
            .def(py::self == py::self,
                 "Compare DataState objects for equality.")
            .def(py::self != py::self,
                 "Compare DataState objects for inequality.")
            .def("__lshift__",
                 (DataState & (DataState::*) (const SampleState&) )
                         & DataState::operator<<,
                 py::is_operator(),
                 "Set the SampleState for the DataState.")
            .def("__lshift__",
                 (DataState & (DataState::*) (const ViewState&) )
                         & DataState::operator<<,
                 py::is_operator(),
                 "Set the ViewState for the DataState.")
            .def("__lshift__",
                 (DataState & (DataState::*) (const InstanceState&) )
                         & DataState::operator<<,
                 py::is_operator(),
                 "Set the InstanceState for the DataState.")
            .def_property(
                    "sample_state",
                    (const SampleState& (DataState::*) () const)
                            & DataState::sample_state,
                    (void (DataState::*)(const SampleState&))
                            & DataState::sample_state,
                    "The SampleState of the DataState.")
            .def_property(
                    "instance_state",
                    (const InstanceState& (DataState::*) () const)
                            & DataState::instance_state,
                    (void (DataState::*)(const InstanceState&))
                            & DataState::instance_state,
                    "The InstanceState of the DataState.")
            .def_property(
                    "view_state",
                    (const ViewState& (DataState::*) () const)
                            & DataState::view_state,
                    (void (DataState::*)(const ViewState&))
                            & DataState::view_state,
                    "The ViewState of the DataState.")
            .def_property_readonly_static(
                    "any",
                    [](py::object&) {
                        return DataState::any();
                    },
                    "Create a DataState with InstanceState.ANY, "
                    "ViewState.ANY, "
                    "and SampleState.ANY")
            .def_property_readonly_static(
                    "new_data",
                    [](py::object&) {
                        return DataState::new_data();
                    },
                    "Create a DataState with InstanceState.ALIVE, "
                    "ViewState.ANY, "
                    "and SampleState.NOT_READ")
            .def_property_readonly_static(
                    "any_data",
                    [](py::object&) {
                        return DataState::any_data();
                    },
                    "Create a DataState with InstanceState.ALIVE, "
                    "ViewState.ANY, "
                    "and SampleState.ANY")
            .def_property_readonly_static(
                    "new_instance",
                    [](py::object&) {
                        return DataState::new_instance();
                    },
                    "Create a DataState with InstanceState.ALIVE, "
                    "ViewState.NEW_VIEW, and SampleState.ANY");

        py::implicitly_convertible<SampleState, DataState>();
        py::implicitly_convertible<ViewState, DataState>();
        py::implicitly_convertible<InstanceState, DataState>();
}


template<>
void process_inits<DataState>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        auto cls = init_mask_type<SampleState, uint32_t>(
                m,
                "SampleState",
                "Create a SampleState with no bits set.",
                false);
        return [cls]() mutable { init_class_defs<SampleState>(cls); };
    });

    l.push_back([m]() mutable {
        auto cls = init_mask_type<ViewState, uint32_t>(
                m,
                "ViewState",
                "Create a ViewState with no bits set.",
                false);
        return [cls]() mutable { init_class_defs<ViewState>(cls); };
    });

    l.push_back([m]() mutable {
        auto cls = init_mask_type<InstanceState, uint32_t>(
                m,
                "InstanceState",
                "Create an InstanceState with no bits set.",
                false);
        return [cls]() mutable { init_class_defs<InstanceState>(cls); };
    });

    l.push_back(
            [m]() mutable { return init_class<DataState>(m, "DataState"); });
}

}  // namespace pyrti
