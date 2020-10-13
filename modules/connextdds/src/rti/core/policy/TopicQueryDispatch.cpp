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
#include <rti/core/policy/CorePolicy.hpp>

using namespace rti::core::policy;

namespace pyrti {

template<>
void init_class_defs(py::class_<TopicQueryDispatch>& cls)
{
    cls.def(py::init<>(), "Creates the default policy.")
            .def(py::init<bool, const dds::core::Duration&, int32_t>(),
                 py::arg("enable"),
                 py::arg("publication_period"),
                 py::arg("samples_per_period"),
                 "Creates a policy with the provided values for enable, "
                 "publication_period and samples_per_period.")
            .def_property(
                    "enable",
                    (bool (TopicQueryDispatch::*)() const)
                            & TopicQueryDispatch::enable,
                    (TopicQueryDispatch & (TopicQueryDispatch::*) (bool) )
                            & TopicQueryDispatch::enable,
                    "Allows this writer to dispatch TopicQueries.")
            .def_property(
                    "publication_period",
                    (dds::core::Duration(TopicQueryDispatch::*)() const)
                            & TopicQueryDispatch::publication_period,
                    (TopicQueryDispatch
                     & (TopicQueryDispatch::*) (const dds::core::Duration&) )
                            & TopicQueryDispatch::publication_period,
                    "The periodic interval at which samples are published.")
            .def_property(
                    "samples_per_period",
                    (int32_t(TopicQueryDispatch::*)() const)
                            & TopicQueryDispatch::samples_per_period,
                    (TopicQueryDispatch & (TopicQueryDispatch::*) (int32_t))
                            & TopicQueryDispatch::samples_per_period,
                    "The maximum number of samples to publish in each "
                    "publication_period.")
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.");
}

template<>
void process_inits<TopicQueryDispatch>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<TopicQueryDispatch>(m, "TopicQueryDispatch");
    });
}

}  // namespace pyrti
