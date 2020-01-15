#include "PyConnext.hpp"
#include <rti/sub/TopicQuery.hpp>
#include <dds/topic/BuiltinTopic.hpp>
#include "PySafeEnum.hpp"

using namespace rti::sub;

template<>
void pyrti::init_class_defs(py::class_<TopicQuerySelection>& cls) {
    cls
        .def(
            py::init<
                const dds::topic::Filter&
            >(),
            py::arg("filter"),
            "Creates a TopicQuerySelection."
        )
        .def(
            py::init<
                const dds::topic::Filter&,
                TopicQuerySelectionKind
            >(),
            py::arg("filter"),
            py::arg("kind"),
            "Creates a TopicQuerySelection with a selection kind."
        )
        .def_property(
            "filter",
            (dds::topic::Filter& (TopicQuerySelection::*)()) &TopicQuerySelection::filter,
            [](TopicQuerySelection& tqs, dds::topic::Filter& f) {
                tqs.filter() = f;
            },
            "The filter."
        )
        .def_property(
            "kind",
            (TopicQuerySelectionKind (TopicQuerySelection::*)() const) &TopicQuerySelection::kind,
            (TopicQuerySelection& (TopicQuerySelection::*)(TopicQuerySelectionKind)) &TopicQuerySelection::kind,
            "Indicates whether the sample selection is limited to cached "
            "samples or not."
        );
}

template<>
void pyrti::init_class_defs(py::class_<TopicQueryData>& cls) {
    cls
        .def_static(
            "create_from_service_request",
            &rti::sub::create_topic_query_data_from_service_request,
            py::arg("service_request"),
            "Creates a TopicQueryData from a ServiceRequest."
        )
        .def_property_readonly(
            "selection",
            &TopicQueryData::selection,
            "The data selection."
        )
        .def_property_readonly(
            "topic_name",
            &TopicQueryData::topic_name,
            "The topic name of the DataReader."
        )
        .def_property_readonly(
            "original_related_reader_guid",
            &TopicQueryData::original_related_reader_guid,
            "Identifies the DataReader that created the TopicQuery."
        );
}

template<>
void pyrti::init_class_defs(py::class_<TopicQuery>& cls) {
    cls
        .def(
            py::init<
                dds::sub::AnyDataReader,
                const TopicQuerySelection&
            >(),
            py::arg("reader"),
            py::arg("selection"),
            "Creates a TopicQuery for a given DataReader."
        )
        .def(
            "close",
            &TopicQuery::close,
            "Deletes and cancels this TopicQuery."
        )
        .def_property_readonly(
            "guid",
            &TopicQuery::guid,
            "The TopicQuery GUID."
        )
        .def_property_readonly(
            "closed",
            &TopicQuery::closed,
            "Indicates whether this TopicQuery has been closed with close()."
        )
        .def_property_readonly(
            "datareader",
            &TopicQuery::datareader,
            "Gets the DataReader associated to this TopicQuery."
        )
        .def(
            "__enter__",
            [](TopicQuery& tq) {
                return tq;
            },
            "Enter a context managed block for a TopicQuery."
        )
        .def(
            "__exit__",
            [](TopicQuery& tq) {
                tq.close();
            },
            "Exit a context managed block for a TopicQuery."
        )
        .def(
            py::self == py::self,
            "Compare DataStateEx objects for equality."
        )
        .def(
            py::self != py::self,
            "Compare DataStateEx objects for inequality."
        );
}

template<>
void pyrti::process_inits<TopicQuery>(py::module& m, pyrti::ClassInitList& l) {
    auto tqsk = init_dds_safe_enum<TopicQuerySelectionKind_def>(m, "TopicQuerySelectionKind");

    py::enum_<TopicQuerySelectionKind::type>(tqsk, "TopicQuerySelectionKind")
        .value(
            "HISTORY_SNAPSHOT",
            TopicQuerySelectionKind::type::HISTORY_SNAPSHOT,
            "[default] Indicates that the TopicQuery may only select samples "
            "that were in the DataWriter cache upon reception."
        )
        .value(
            "CONTINUOUS",
            TopicQuerySelectionKind::type::CONTINUOUS,
            "Indicates that the TopicQuery may continue selecting samples "
            "published after its reception."
            "\n\n"
            "The subscribing application must explicitly delete the "
            "TopicQuery (see TopicQuery.close()) to signal the DataWriters "
            "to stop publishing samples for it."
        )
        .export_values();

    l.push_back(
        [m]() mutable {
            return pyrti::init_class<TopicQuerySelection>(m, "TopicQuerySelection");
        }
    );

    l.push_back(
        [m]() mutable {
            return pyrti::init_class<TopicQueryData>(m, "TopicQueryData");
        }
    );

    l.push_back(
        [m]() mutable {
            return pyrti::init_class<TopicQuery>(m, "TopicQuery");
        }
    );
}
