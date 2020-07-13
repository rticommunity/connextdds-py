#include "PyConnext.hpp"
#include "PyMaskType.hpp"
#include <rti/sub/status/DataStateEx.hpp>

using namespace rti::sub::status;

namespace pyrti {

template<>
void init_class_defs(py::class_<StreamKind>& cls)
{
    cls.def_static(
               "live",
               &rti::sub::status::StreamKind::live,
               "Returns the StreamKind that selects the live stream.")
            .def_static(
                    "topic_query",
                    &rti::sub::status::StreamKind::topic_query,
                    "Returns the StreamKind that selects the topic-query "
                    "stream.")
            .def_static(
                    "any",
                    &rti::sub::status::StreamKind::any,
                    "Returns the StreamKind that selects either stream.");
}

template<>
void init_class_defs(py::class_<DataStateEx>& cls)
{
    cls.def(py::init<>(),
            "Create a DataStateEx with InstanceState.any(), ViewState.any(), "
            "SampleState.any(), and StreamKind.any().")
            .def(py::init<const StreamKind&>(),
                 py::arg("stream_kind"),
                 "Create a DataStateEx with InstanceState.any(), "
                 "ViewState.any(), "
                 "SampleState.any() and the provided StreamKind.")
            .def(py::init<const dds::sub::status::DataState&>(),
                 py::arg("data_state"),
                 "Create a DataStateEx with the provided SampleState, "
                 "ViewState, "
                 "InstanceState, and StreamKind.any()")
            .def(py::init<
                         const dds::sub::status::DataState&,
                         const StreamKind&>(),
                 py::arg("data_state"),
                 py::arg("stream_kind"),
                 "Create a DataStateEx with the provided SampleState, "
                 "ViewState, "
                 "InstanceState, and StreamKind.")
            .def_property_readonly(
                    "data_state",
                    &DataStateEx::data_state,
                    "Access the view, sample and instance states.")
            .def_property(
                    "stream_kind",
                    (const StreamKind& (DataStateEx::*) () const)
                            & DataStateEx::stream_kind,
                    (void (DataStateEx::*)(const StreamKind&))
                            & DataStateEx::stream_kind,
                    "The StreamKind.")
            .def_property(
                    "sample_state",
                    (const dds::sub::status::SampleState& (DataStateEx::*) ()
                             const)
                            & DataStateEx::sample_state,
                    (void (DataStateEx::*)(
                            const dds::sub::status::SampleState&))
                            & DataStateEx::sample_state,
                    "The SampleState of the DataStateEx.")
            .def_property(
                    "instance_state",
                    (const dds::sub::status::InstanceState& (DataStateEx::*) ()
                             const)
                            & DataStateEx::instance_state,
                    (void (DataStateEx::*)(
                            const dds::sub::status::InstanceState&))
                            & DataStateEx::instance_state,
                    "The InstanceState of the DataStateEx.")
            .def_property(
                    "view_state",
                    (const dds::sub::status::ViewState& (DataStateEx::*) ()
                             const)
                            & DataStateEx::view_state,
                    (void (DataStateEx::*)(const dds::sub::status::ViewState&))
                            & DataStateEx::view_state,
                    "The ViewState of the DataStateEx.")
            .def_static(
                    "any",
                    &DataStateEx::any,
                    "Create a DataStateEx with InstanceState.any(), "
                    "ViewState.any(), "
                    "SampleState.any(), and StreamKind.any()")
            .def_static(
                    "new_data",
                    &DataStateEx::new_data,
                    "Create a DataStateEx with InstanceState.alive(), "
                    "ViewState.any(), "
                    "SampleState.not_read()")
            .def_static(
                    "any_data",
                    &DataStateEx::any_data,
                    "Create a DataStateEx with InstanceState.alive(), "
                    "ViewState.any(), "
                    "SampleState.any(), and StreamKind.any()")
            .def_static(
                    "new_instance",
                    &DataStateEx::new_instance,
                    "Create a DataStateEx with InstanceState.alive(), "
                    "ViewState.new_view(), SampleState.any() and "
                    "StreamKind.any()")
            .def(py::self == py::self,
                 "Compare DataStateEx objects for equality.")
            .def(py::self != py::self,
                 "Compare DataStateEx objects for inequality.")
            .def("__lshift__",
                 (DataStateEx & (DataStateEx::*) (const StreamKind&) )
                         & DataStateEx::operator<<,
                 py::is_operator(),
                 "Set the StreamKind for the DataStateEx.")
            .def("__lshift__",
                 (dds::sub::status::DataState
                  & (DataStateEx::*) (const dds::sub::status::SampleState&) )
                         & DataStateEx::operator<<,
                 py::is_operator(),
                 "Set the SampleState for the DataState.")
            .def("__lshift__",
                 (dds::sub::status::DataState
                  & (DataStateEx::*) (const dds::sub::status::ViewState&) )
                         & DataStateEx::operator<<,
                 py::is_operator(),
                 "Set the ViewState for the DataState.")
            .def("__lshift__",
                 (dds::sub::status::DataState
                  & (DataStateEx::*) (const dds::sub::status::InstanceState&) )
                         & DataStateEx::operator<<,
                 py::is_operator(),
                 "Set the InstanceState for the DataState.");

    py::implicitly_convertible<dds::sub::status::DataState, DataStateEx>();
}

template<>
void process_inits<DataStateEx>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        auto cls = init_mask_type<StreamKind, uint32_t>(
                m,
                "StreamKind",
                "Create a StreamKind with nothing enabled.");
        return [cls]() mutable { init_class_defs<StreamKind>(cls); };
    });

    l.push_back([m]() mutable {
        return init_class<DataStateEx>(m, "DataStateEx");
    });
}

}  // namespace pyrti
