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
#include <rti/sub/status/DataStateEx.hpp>
#include "PyMaskType.hpp"

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
                    [](const DataStateEx& ds) { return ds.sample_state(); },
                    [](DataStateEx& ds,
                       const dds::sub::status::SampleState& ss) {
                        ds.sample_state(ss);
                    },
                    "The SampleState of the DataStateEx.")
            .def_property(
                    "instance_state",
                    [](const DataStateEx& ds) { return ds.instance_state(); },
                    [](DataStateEx& ds, dds::sub::status::InstanceState& is) {
                        ds.instance_state(is);
                    },
                    "The InstanceState of the DataStateEx.")
            .def_property(
                    "view_state",
                    [](const DataStateEx& ds) { return ds.view_state(); },
                    [](DataStateEx& ds, dds::sub::status::ViewState& vs) {
                        ds.view_state(vs);
                    },
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
            .def(
                    "__lshift__",
                    [](DataStateEx& ds,
                       const dds::sub::status::SampleState& ss) {
                        return ds << ss;
                    },
                    py::is_operator(),
                    "Set the SampleState for the DataState.")
            .def(
                    "__lshift__",
                    [](DataStateEx& ds, const dds::sub::status::ViewState& vs) {
                        return ds << vs;
                    },
                    py::is_operator(),
                    "Set the ViewState for the DataState.")
            .def(
                    "__lshift__",
                    [](DataStateEx& ds,
                       const dds::sub::status::InstanceState& is) {
                        return ds << is;
                    },
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
