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
#include <rti/rti.hpp>
#include <rti/topic/cdr/GenericTypePluginFactory.hpp>
#include <rti/topic/cdr/GenericTypePlugin.hpp>

using namespace rti::topic;

void init_namespace_rti_topic(py::module& m, pyrti::ClassInitList& l, pyrti::DefInitVector&)
{
    pyrti::process_inits<ContentFilterBase>(m, l);
    pyrti::process_inits<ExpressionProperty>(m, l);
    pyrti::process_inits<FilterSampleInfo>(m, l);
    pyrti::process_inits<PrintFormatProperty>(m, l);
    pyrti::process_inits<ServiceRequest>(m, l);

    // Factory used to create the DynamicTypes used by the IDL type plugins.
    pyrti::process_inits<rti::topic::cdr::GenericTypePluginFactory>(m, l);

    // Define the IDL-based dds.Topic, dds.DataWriter, dds.DataReader types
    pyrti::process_inits<rti::topic::cdr::CSampleWrapper>(m, l);
}