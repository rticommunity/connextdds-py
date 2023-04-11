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
#include <rti/core/status/Status.hpp>

using namespace rti::core::status;

namespace pyrti {

template<>
void init_class_defs(py::class_<DataWriterCacheStatus>& cls)
{
    cls.def_property_readonly(
            "sample_count",
            &DataWriterCacheStatus::sample_count,
            "Number of samples in the DataWriter's queue, including "
            "unregister and dispose samples.");
    cls.def_property_readonly(
            "sample_count_peak",
            &DataWriterCacheStatus::sample_count_peak,
            "Highest number of samples in the writer's queue over the "
            "lifetime of the writer.");
    cls.def_property_readonly(
            "alive_instance_count",
            &DataWriterCacheStatus::alive_instance_count,
            "Number of alive instances in the DataWriter's queue.");
    cls.def_property_readonly(
            "alive_instance_count_peak",
            &DataWriterCacheStatus::alive_instance_count_peak,
            "Highest number of alive instances in the writer's queue over the "
            "lifetime of the writer.");
    cls.def_property_readonly(
            "disposed_instance_count",
            &DataWriterCacheStatus::disposed_instance_count,
            "Number of disposed instances in the DataWriter's queue.");
    cls.def_property_readonly(
            "disposed_instance_count_peak",
            &DataWriterCacheStatus::disposed_instance_count_peak,
            "Highest number of disposed instances in the writer's queue "
            "over the lifetime of the writer.");
    cls.def_property_readonly(
            "unregistered_instance_count",
            &DataWriterCacheStatus::unregistered_instance_count,
            "Number of unregistered instances in the DataWriter's queue.");
    cls.def_property_readonly(
            "unregistered_instance_count_peak",
            &DataWriterCacheStatus::unregistered_instance_count_peak,
            "Highest number of unregistered instances in the writer's "
            "queue over the lifetime of the writer.");
    
}

template<>
void process_inits<DataWriterCacheStatus>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<DataWriterCacheStatus>(m, "DataWriterCacheStatus");
    });
}

}  // namespace pyrti
