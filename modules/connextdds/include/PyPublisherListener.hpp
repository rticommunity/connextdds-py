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

#pragma once

#include "PyConnext.hpp"
#include <dds/pub/PublisherListener.hpp>
#include "PyAnyDataWriterListener.hpp"
#include "PyEntity.hpp"

namespace pyrti {

class PyPublisherListener : public dds::pub::PublisherListener,
                            public PyAnyDataWriterListener {
public:
    using dds::pub::PublisherListener::PublisherListener;
    using PyAnyDataWriterListener::PyAnyDataWriterListener;

    virtual ~PyPublisherListener()
    {
    }
};

class PyNoOpPublisherListener : public PyPublisherListener,
                                public PyNoOpAnyDataWriterListener {
public:
    using PyPublisherListener::PyPublisherListener;

    using PyNoOpAnyDataWriterListener::on_application_acknowledgment;
    using PyNoOpAnyDataWriterListener::on_instance_replaced;
    using PyNoOpAnyDataWriterListener::on_liveliness_lost;
    using PyNoOpAnyDataWriterListener::on_offered_deadline_missed;
    using PyNoOpAnyDataWriterListener::on_offered_incompatible_qos;
    using PyNoOpAnyDataWriterListener::on_publication_matched;
    using PyNoOpAnyDataWriterListener::on_reliable_reader_activity_changed;
    using PyNoOpAnyDataWriterListener::on_reliable_writer_cache_changed;
    using PyNoOpAnyDataWriterListener::on_service_request_accepted;
    using PyNoOpAnyDataWriterListener::on_sample_removed;

    virtual ~PyNoOpPublisherListener()
    {
    }
};

}  // namespace pyrti