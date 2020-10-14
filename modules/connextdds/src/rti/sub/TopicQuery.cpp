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
#include <rti/sub/TopicQuery.hpp>
#include <dds/topic/BuiltinTopic.hpp>
#include "PySafeEnum.hpp"

using namespace rti::sub;

namespace pyrti {

template<>
void init_class_defs(py::class_<TopicQuerySelection>& cls)
{
    cls.def(py::init<const dds::topic::Filter&>(),
            py::arg("filter"),
            "Creates a TopicQuerySelection.")
#if rti_connext_version_gte(6, 0, 0)
            .def(py::init<const dds::topic::Filter&, TopicQuerySelectionKind>(),
                 py::arg("filter"),
                 py::arg("kind"),
                 "Creates a TopicQuerySelection with a selection kind.")
#else
            .def(py::init<const dds::topic::Filter&>(),
                 py::arg("filter"),
                 "Creates a TopicQuerySelection.")
#endif
            .def_property(
                    "filter",
                    (dds::topic::Filter & (TopicQuerySelection::*) ())
                            & TopicQuerySelection::filter,
                    [](TopicQuerySelection& tqs, dds::topic::Filter& f) {
                        tqs.filter() = f;
                    },
                    "The filter.")
#if rti_connext_version_gte(6, 0, 0)
            .def_property(
                    "kind",
                    (TopicQuerySelectionKind(TopicQuerySelection::*)() const)
                            & TopicQuerySelection::kind,
                    (TopicQuerySelection
                     & (TopicQuerySelection::*) (TopicQuerySelectionKind))
                            & TopicQuerySelection::kind,
                    "Indicates whether the sample selection is limited to "
                    "cached "
                    "samples or not.")
#endif
            ;

    py::implicitly_convertible<
            dds::topic::Filter,
            rti::sub::TopicQuerySelection>();
}

template<>
void init_class_defs(py::class_<TopicQueryData>& cls)
{
    cls.def_static(
               "create_from_service_request",
               &rti::sub::create_topic_query_data_from_service_request,
               py::arg("service_request"),
               "Creates a TopicQueryData from a ServiceRequest.")
            .def_property_readonly(
                    "selection",
                    &TopicQueryData::selection,
                    "The data selection.")
            .def_property_readonly(
                    "topic_name",
                    [](const TopicQueryData& tqd) {
                        return tqd.topic_name().to_std_string();
                    },
                    "The topic name of the DataReader.")
            .def_property_readonly(
                    "original_related_reader_guid",
                    &TopicQueryData::original_related_reader_guid,
                    "Identifies the DataReader that created the TopicQuery.");
}

template<>
void init_class_defs(py::class_<TopicQuery>& cls)
{
    cls.def(py::init([](PyIAnyDataReader& dr, const TopicQuerySelection& tqs) {
                return TopicQuery(dr.get_any_datareader(), tqs);
            }),
            py::arg("reader"),
            py::arg("selection"),
            "Creates a TopicQuery for a given DataReader.")
            .def("close",
                 &TopicQuery::close,
                 "Deletes and cancels this TopicQuery.")
            .def_property_readonly(
                    "guid",
                    &TopicQuery::guid,
                    "The TopicQuery GUID.")
            .def_property_readonly(
                    "closed",
                    &TopicQuery::closed,
                    "Indicates whether this TopicQuery has been closed with "
                    "close().")
            .def_property_readonly(
                    "datareader",
                    [](TopicQuery& tq) {
                        return PyAnyDataReader(tq.datareader());
                    },
                    "Gets the DataReader associated to this TopicQuery.")
            .def(
                    "retain",
                    [](TopicQuery& tq) { tq.retain(); },
                    "Disable automatic destruction of this TopicQuery.")
            .def("unretain", [](TopicQuery& tq) { tq.delegate()->unretain(); })
            .def(
                    "__enter__",
                    [](TopicQuery& tq) { return tq; },
                    "Enter a context managed block for a TopicQuery.")
            .def(
                    "__exit__",
                    [](TopicQuery& tq, py::object, py::object, py::object) {
                        tq.close();
                    },
                    "Exit a context managed block for a TopicQuery.")
            .def(py::self == py::self,
                 "Compare DataStateEx objects for equality.")
            .def(py::self != py::self,
                 "Compare DataStateEx objects for inequality.")
            .def_static(
                    "use_reader_content_filter",
                    [](PyIAnyDataReader& adr) {
                        auto dr = adr.get_any_datareader();
                        return TopicQuery::UseReaderContentFilter(dr);
                    },
                    py::arg("reader"),
                    "Create a TopicQuery with a DataReader's content filter.")
            .def_static(
                    "select_all",
                    [](PyIAnyDataReader& adr) {
                        auto dr = adr.get_any_datareader();
                        return TopicQuery::SelectAll(dr);
                    },
                    py::arg("reader"),
                    "Create a TopicQuery that requests all data.")
            .def_static(
                    "find",
                    [](PyIAnyDataReader& adr, rti::core::Guid& guid) {
                        dds::core::optional<TopicQuery> retval;
                        auto dr = adr.get_any_datareader();
                        auto tq = rti::sub::find_topic_query(dr, guid);
                        if (dds::core::null != tq) {
                            retval = tq;
                        }
                        return retval;
                    });
}

template<>
void process_inits<TopicQuery>(py::module& m, ClassInitList& l)
{
#if rti_connext_version_gte(6, 0, 0)
    init_dds_safe_enum<TopicQuerySelectionKind_def>(
            m,
            "TopicQuerySelectionKind",
            [](py::object& o) {
                py::enum_<TopicQuerySelectionKind::type>(o, "TopicQuerySelectionKind")
                        .value("HISTORY_SNAPSHOT",
                               TopicQuerySelectionKind::type::HISTORY_SNAPSHOT,
                               "[default] Indicates that the TopicQuery may "
                               "only select samples "
                               "that were in the DataWriter cache upon "
                               "reception.")
                        .value("CONTINUOUS",
                               TopicQuerySelectionKind::type::CONTINUOUS,
                               "Indicates that the TopicQuery may continue "
                               "selecting samples "
                               "published after its reception."
                               "\n\n"
                               "The subscribing application must explicitly "
                               "delete the "
                               "TopicQuery (see TopicQuery.close()) to signal "
                               "the DataWriters "
                               "to stop publishing samples for it.")
                        .export_values();
            });
#endif

    l.push_back([m]() mutable {
        return init_class<TopicQuerySelection>(m, "TopicQuerySelection");
    });

    l.push_back([m]() mutable {
        return init_class<TopicQueryData>(m, "TopicQueryData");
    });

    l.push_back(
            [m]() mutable { return init_class<TopicQuery>(m, "TopicQuery"); });
}

}  // namespace pyrti
