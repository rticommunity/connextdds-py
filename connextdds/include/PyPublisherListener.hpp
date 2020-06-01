#pragma once

#include "PyConnext.hpp"
#include <dds/pub/PublisherListener.hpp>
#include "PyAnyDataWriterListener.hpp"
#include "PyEntity.hpp"

namespace pyrti {

class PyPublisherListener : 
    public dds::pub::PublisherListener,
    public PyAnyDataWriterListener {
public:
    using dds::pub::PublisherListener::PublisherListener;
    using PyAnyDataWriterListener::PyAnyDataWriterListener;

    virtual
    ~PyPublisherListener() {}
};

class PyNoOpPublisherListener : 
    public PyPublisherListener,
    public PyNoOpAnyDataWriterListener {
public:
    using PyPublisherListener::PyPublisherListener;
    
    using PyNoOpAnyDataWriterListener::on_offered_deadline_missed;
    using PyNoOpAnyDataWriterListener::on_offered_incompatible_qos;
    using PyNoOpAnyDataWriterListener::on_liveliness_lost;
    using PyNoOpAnyDataWriterListener::on_publication_matched;
    using PyNoOpAnyDataWriterListener::on_reliable_writer_cache_changed;
    using PyNoOpAnyDataWriterListener::on_reliable_reader_activity_changed;
    using PyNoOpAnyDataWriterListener::on_instance_replaced;
    using PyNoOpAnyDataWriterListener::on_application_acknowledgment;
    using PyNoOpAnyDataWriterListener::on_service_request_accepted;

    virtual
    ~PyNoOpPublisherListener() {}
};

}