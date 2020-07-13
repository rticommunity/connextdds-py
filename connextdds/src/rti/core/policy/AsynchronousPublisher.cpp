#include "PyConnext.hpp"
#include <rti/core/policy/CorePolicy.hpp>

using namespace rti::core;
using namespace rti::core::policy;

namespace pyrti {

template<>
void init_class_defs(py::class_<AsynchronousPublisher>& cls) {
    cls
        .def(
            py::init<>(),
            "Create a default AsynchronousPublisher QoS policy."
        )
        .def_property(
            "disable_asynchronous_write",
            (bool (AsynchronousPublisher::*)() const) & AsynchronousPublisher::disable_asynchronous_write,
            (AsynchronousPublisher& (AsynchronousPublisher::*)(bool)) &AsynchronousPublisher::disable_asynchronous_write,
            "Get/set the asynchronous write capability for this policy."
        )
        .def_property(
            "thread",
            (ThreadSettings& (AsynchronousPublisher::*)()) &AsynchronousPublisher::thread,
            (AsynchronousPublisher& (AsynchronousPublisher::*)(const ThreadSettings&)) &AsynchronousPublisher::thread,
            "Get/set the settings associated with the asynchronous publisher "
            "thread."
        )
        .def_property(
            "disable_asynchronous_batch",
            (bool (AsynchronousPublisher::*)() const) &AsynchronousPublisher::disable_asynchronous_batch,
            (AsynchronousPublisher& (AsynchronousPublisher::*)(bool)) &AsynchronousPublisher::disable_asynchronous_batch,
            "Get/set the asynchronous batch flushing capability for this "
            "policy."
        )
        .def_property(
            "asynchronous_batch_thread",
            (ThreadSettings& (AsynchronousPublisher::*)()) &AsynchronousPublisher::asynchronous_batch_thread,
            (AsynchronousPublisher& (AsynchronousPublisher::*)(const ThreadSettings&)) &AsynchronousPublisher::asynchronous_batch_thread,
            "Get/set the settings associated with the asynchronous batch "
            "flush thread."
        )
        .def_property(
            "disable_topic_query_publication",
            (bool (AsynchronousPublisher::*)() const) &AsynchronousPublisher::disable_topic_query_publication,
            (AsynchronousPublisher& (AsynchronousPublisher::*)(bool)) &AsynchronousPublisher::disable_topic_query_publication,
            "Get/set the topic query publication capability for this policy."
        )
        .def_property(
            "topic_query_publication_thread",
            (ThreadSettings& (AsynchronousPublisher::*)()) &AsynchronousPublisher::topic_query_publication_thread,
            (AsynchronousPublisher& (AsynchronousPublisher::*)(const ThreadSettings&)) &AsynchronousPublisher::topic_query_publication_thread,
            "Get/set the settings associated with the topic query publication "
            "thread."
        )
        .def_static(
            "enabled",
            &AsynchronousPublisher::Enabled,
            py::arg("enable_async_batch") = false,
            "Create an AsynchronousPublisher QoS policy with asynchronous "
            "write (and optionally asynchronous batch flushing) enabled."
        )
        .def_static(
            "disabled",
            &AsynchronousPublisher::Disabled,
            "Create an AsynchronousPublisher QoS policy with asynchronous "
            "publication completely disabled."
        )
        .def(
            py::self == py::self,
            "Test for equality."
        )
        .def(
            py::self != py::self,
            "Test for inequality."
        );
}

template<>
void process_inits<AsynchronousPublisher>(py::module& m, ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return init_class<AsynchronousPublisher>(m, "AsynchronousPublisher");
        }
    );
}

}
