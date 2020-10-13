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
#include <rti/pub/AcknowledgmentInfo.hpp>

using namespace rti::pub;

namespace pyrti {

template<>
void init_class_defs(py::class_<AcknowledgmentInfo>& cls)
{
    cls.def_property_readonly(
               "subscription_handle",
               &AcknowledgmentInfo::subscription_handle,
               "Gets the subscription handle of the acknowledging DataReader")
            .def_property_readonly(
                    "sample_identity",
                    &AcknowledgmentInfo::sample_identity,
                    "Get the identity of the sample being acknowledged.")
            .def_property_readonly(
                    "valid_response_data",
                    &AcknowledgmentInfo::valid_response_data,
                    "Flag indicating validity of the use response data in the "
                    "acknowledgment.")
            .def_property_readonly(
                    "response_data",
                    &AcknowledgmentInfo::response_data,
                    "User data payload of application-level acknowledgment.");
}

template<>
void process_inits<AcknowledgmentInfo>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<AcknowledgmentInfo>(m, "AcknowledgmentInfo");
    });
}

}  // namespace pyrti
