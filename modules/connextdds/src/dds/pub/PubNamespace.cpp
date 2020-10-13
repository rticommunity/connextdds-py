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
#include "PyNamespaces.hpp"
#include <dds/dds.hpp>
#include <dds/pub/SuspendedPublication.hpp>

using namespace dds::pub;

void init_namespace_dds_pub(py::module& m, pyrti::ClassInitList& l)
{
    pyrti::process_inits<AnyDataWriter>(m, l);
    pyrti::process_inits<AnyDataWriterListener>(m, l);
    pyrti::process_inits<CoherentSet>(m, l);
    pyrti::process_inits<Publisher>(m, l);
    pyrti::process_inits<PublisherListener>(m, l);
    pyrti::process_inits<SuspendedPublication>(m, l);

    init_namespace_dds_pub_qos(m, l);
}
