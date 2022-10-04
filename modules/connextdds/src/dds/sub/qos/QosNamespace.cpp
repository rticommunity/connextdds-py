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
#include <dds/dds.hpp>

using namespace dds::sub::qos;

void init_namespace_dds_sub_qos(py::module& m, pyrti::ClassInitList& l, pyrti::DefInitVector&)
{
    pyrti::process_inits<DataReaderQos>(m, l);
    pyrti::process_inits<SubscriberQos>(m, l);
}
