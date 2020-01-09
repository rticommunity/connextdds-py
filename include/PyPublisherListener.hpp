#pragma once

#include "PyConnext.hpp"
#include <dds/pub/PublisherListener.hpp>
#include "PyAnyDataWriterListener.hpp"
#include "PyEntity.hpp"

namespace pyrti {
    class PyPublisherListener : 
        public dds::pub::PublisherListener,
        public pyrti::PyAnyDataWriterListener {
    public:
        using dds::pub::PublisherListener::PublisherListener;
        using pyrti::PyAnyDataWriterListener::PyAnyDataWriterListener;

        /*using pyrti::PyAnyDataWriterListener::on_offered_deadline_missed;
        using pyrti::PyAnyDataWriterListener::on_offered_incompatible_qos;
        using pyrti::PyAnyDataWriterListener::on_liveliness_lost;
        using pyrti::PyAnyDataWriterListener::on_publication_matched;
        using pyrti::PyAnyDataWriterListener::on_reliable_writer_cache_changed;
        using pyrti::PyAnyDataWriterListener::on_reliable_reader_activity_changed;
        using pyrti::PyAnyDataWriterListener::on_instance_replaced;
        using pyrti::PyAnyDataWriterListener::on_application_acknowledgment;
        using pyrti::PyAnyDataWriterListener::on_service_request_accepted;*/

        virtual
        ~PyPublisherListener() {}
    };

    class PyNoOpPublisherListener : 
        public pyrti::PyPublisherListener,
        public pyrti::PyNoOpAnyDataWriterListener {
    public:
        using pyrti::PyPublisherListener::PyPublisherListener;
        using pyrti::PyNoOpAnyDataWriterListener::PyNoOpAnyDataWriterListener;
        
        using pyrti::PyNoOpAnyDataWriterListener::on_offered_deadline_missed;
        using pyrti::PyNoOpAnyDataWriterListener::on_offered_incompatible_qos;
        using pyrti::PyNoOpAnyDataWriterListener::on_liveliness_lost;
        using pyrti::PyNoOpAnyDataWriterListener::on_publication_matched;
        using pyrti::PyNoOpAnyDataWriterListener::on_reliable_writer_cache_changed;
        using pyrti::PyNoOpAnyDataWriterListener::on_reliable_reader_activity_changed;
        using pyrti::PyNoOpAnyDataWriterListener::on_instance_replaced;
        using pyrti::PyNoOpAnyDataWriterListener::on_application_acknowledgment;
        using pyrti::PyNoOpAnyDataWriterListener::on_service_request_accepted;

        virtual
        ~PyNoOpPublisherListener() {}
    };
}