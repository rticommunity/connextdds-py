#include "PyConnext.hpp"
#include "PySafeEnum.hpp"
#include <rti/pub/FlowController.hpp>

using namespace rti::pub;

namespace pyrti {

template<>
void init_class_defs(py::class_<FlowController>& cls)
{
    cls.def(py::init([](PyDomainParticipant& dp,
                        std::string& n,
                        const FlowControllerProperty& fcp) {
                return FlowController(dp, n, fcp);
            }),
            py::arg("participant"),
            py::arg("name"),
            py::arg_v(
                    "token_bucket",
                    FlowControllerProperty(),
                    "FlowControllerProperty()"),
            "Creates a FlowController with specific properties.")
            .def_property_readonly(
                    "name",
                    &FlowController::name,
                    "The name of this FlowController.")
            .def_property_readonly(
                    "participant",
                    [](FlowController& fc) {
                        dds::domain::DomainParticipant p = fc.participant();
                        return PyDomainParticipant(p);
                    },
                    "The participant of this FlowController.")
            .def_property_readonly(
                    "property",
                    (FlowControllerProperty(FlowController::*)() const)
                            & FlowController::property,
                    "The configuration of this FlowController.")
            .def("trigger_flow",
                 &FlowController::trigger_flow,
                 "Provides an external way to trigger a FlowController.")
            .def("retain",
                 &FlowController::retain,
                 "Disables the automatic destruction of this object.")
            .def("close",
                 &FlowController::close,
                 "Manually destroys this object.")
            .def_property_readonly(
                    "closed",
                    &FlowController::closed,
                    "Whether this FlowController has been closed.")
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.");
}

template<>
void init_class_defs(py::class_<FlowControllerProperty>& cls)
{
    cls.def(py::init<>(),
            "Creates a FlowControllerProperty with earliest-deadline-first "
            "scheduling policy and default token-bucket configuration.")
            .def(py::init<
                         FlowControllerSchedulingPolicy::type,
                         const FlowControllerTokenBucketProperty&>(),
                 py::arg("scheduling_policy"),
                 py::arg_v(
                         "token_bucket",
                         FlowControllerTokenBucketProperty(),
                         "FlowControllerTokenBucketProperty()"),
                 "Creates a FlowControllerProperty with the specified "
                 "scheduling "
                 "policy and token-bucket configuration.")
            .def_property(
                    "scheduling_policy",
                    (FlowControllerSchedulingPolicy::type(
                            FlowControllerProperty::*)() const)
                            & FlowControllerProperty::scheduling_policy,
                    (FlowControllerProperty
                     & (FlowControllerProperty::*) (FlowControllerSchedulingPolicy::
                                                            type))
                            & FlowControllerProperty::scheduling_policy,
                    "The scheduling policy.")
            .def_property(
                    "token_bucket",
                    (FlowControllerTokenBucketProperty
                     & (FlowControllerProperty::*) ())
                            & FlowControllerProperty::token_bucket,
                    [](FlowControllerProperty& fcp,
                       FlowControllerTokenBucketProperty& prop) {
                        fcp.token_bucket() = prop;
                    },
                    "The token-bucket configuration")
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.");
}

template<>
void init_class_defs(py::class_<FlowControllerTokenBucketProperty>& cls)
{
    cls.def(py::init<
                    int32_t,
                    int32_t,
                    int32_t,
                    const dds::core::Duration&,
                    int32_t>(),
            py::arg_v(
                    "max_tokens",
                    dds::core::LENGTH_UNLIMITED,
                    "LENGTH_UNLIMITED"),
            py::arg_v(
                    "tokens_added_per_period",
                    dds::core::LENGTH_UNLIMITED,
                    "LENGTH_UNLIMITED"),
            py::arg("tokens_leaked_per_period") = 0,
            py::arg_v("period", dds::core::Duration(1, 0), "Duration(1, 0)"),
            py::arg_v(
                    "bytes_per_token",
                    dds::core::LENGTH_UNLIMITED,
                    "LENGTH_UNLIMITED"),
            "Initializes the properties.")
            .def_property(
                    "max_tokens",
                    (int32_t(FlowControllerTokenBucketProperty::*)() const)
                            & FlowControllerTokenBucketProperty::max_tokens,
                    (FlowControllerTokenBucketProperty
                     & (FlowControllerTokenBucketProperty::*) (int32_t))
                            & FlowControllerTokenBucketProperty::max_tokens,
                    "The maximum number of tokens that can accumulate in the "
                    "token "
                    "bucket.")
            .def_property(
                    "tokens_added_per_period",
                    (int32_t(FlowControllerTokenBucketProperty::*)() const)
                            & FlowControllerTokenBucketProperty::
                                    tokens_added_per_period,
                    (FlowControllerTokenBucketProperty
                     & (FlowControllerTokenBucketProperty::*) (int32_t))
                            & FlowControllerTokenBucketProperty::
                                    tokens_added_per_period,
                    "The number of tokens added to the token bucket per "
                    "specified "
                    "period.")
            .def_property(
                    "tokens_leaked_per_period",
                    (int32_t(FlowControllerTokenBucketProperty::*)() const)
                            & FlowControllerTokenBucketProperty::
                                    tokens_leaked_per_period,
                    (FlowControllerTokenBucketProperty
                     & (FlowControllerTokenBucketProperty::*) (int32_t))
                            & FlowControllerTokenBucketProperty::
                                    tokens_leaked_per_period,
                    "The number of tokens removed from the token bucket per "
                    "specified "
                    "period.")
            .def_property(
                    "period",
                    (dds::core::Duration(FlowControllerTokenBucketProperty::*)()
                             const)
                            & FlowControllerTokenBucketProperty::period,
                    (FlowControllerTokenBucketProperty
                     & (FlowControllerTokenBucketProperty::*) (const dds::core::
                                                                       Duration&) )
                            & FlowControllerTokenBucketProperty::period,
                    "The period for adding tokens to and removing tokens from "
                    "the "
                    "bucket.")
            .def_property(
                    "bytes_per_token",
                    (int32_t(FlowControllerTokenBucketProperty::*)() const)
                            & FlowControllerTokenBucketProperty::
                                    bytes_per_token,
                    (FlowControllerTokenBucketProperty
                     & (FlowControllerTokenBucketProperty::*) (int32_t))
                            & FlowControllerTokenBucketProperty::
                                    bytes_per_token,
                    "The maximum number of bytes allowed to send for each "
                    "token "
                    "available.")
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.");
}

template<>
void process_inits<FlowController>(py::module& m, ClassInitList& l)
{
    init_dds_safe_enum<FlowControllerSchedulingPolicy_def>(
            m,
            "FlowControllerSchedulingPolicy",
            [](py::object& o) {
                py::enum_<FlowControllerSchedulingPolicy::type>(o, "Enum")
                        .value("ROUND_ROBIN",
                               FlowControllerSchedulingPolicy::type::
                                       ROUND_ROBIN,
                               "Indicates to flow control in a round-robin "
                               "fashion."
                               "\n\n"
                               "Whenever tokens become available, the flow "
                               "controller "
                               "distributes the tokens uniformly across all of "
                               "its (non-empty) "
                               "destination queues. No destinations are "
                               "prioritized. Instead, "
                               "all destinations are treated equally and are "
                               "serviced in a "
                               "round-robin fashion.")
                        .value("EARLIEST_DEADLINE_FIRST",
                               FlowControllerSchedulingPolicy::type::
                                       EARLIEST_DEADLINE_FIRST,
                               "Indicates to flow control in an "
                               "earliest-deadline-first fashion."
                               "\n\n"
                               "A sample's deadline is determined by the time "
                               "it was written "
                               "plus the latency budget of the DataWriter at "
                               "the time of the "
                               "write call (as specified in the "
                               "LatencyBudget). The relative "
                               "priority of a flow controller's destination "
                               "queue is determined "
                               "by the earliest deadline across all samples it "
                               "contains."
                               "\n\n"
                               "When tokens become available, the "
                               "FlowController distributes "
                               "tokens to the destination queues in order of "
                               "their deadline "
                               "priority. In other words, the queue containing "
                               "the sample with "
                               "the earliest deadline is serviced first. The "
                               "number of tokens "
                               "granted equals the number of tokens required "
                               "to send the first "
                               "sample in the queue. Note that the priority of "
                               "a queue may "
                               "change as samples are sent (i.e. removed from "
                               "the queue). If a "
                               "sample must be sent to multiple destinations "
                               "or two samples have "
                               "an equal deadline value, the corresponding "
                               "destination queues "
                               "are serviced in a round-robin fashion."
                               "\n\n"
                               "Hence, under the default "
                               "LatencyBudget::duration setting, an "
                               "EDF_FLOW_CONTROLLER_SCHED_POLICY "
                               "FlowController preserves the "
                               "order in which the user calls "
                               "DataWriter.write() across the "
                               "DataWriters associated with the flow "
                               "controller."
                               "\n\n"
                               "Since the LatencyBudget is mutable, a sample "
                               "written second may "
                               "contain an earlier deadline than the sample "
                               "written first if the "
                               "LatencyBudget.duration value is sufficiently "
                               "decreased in "
                               "between writing the two samples. In that case, "
                               "if the first "
                               "sample is not yet written (still in queue "
                               "waiting for its turn), "
                               "it inherits the priority corresponding to the "
                               "(earlier) deadline "
                               "from the second sample."
                               "\n\n"
                               "In other words, the priority of a destination "
                               "queue is always "
                               "determined by the earliest deadline among all "
                               "samples contained "
                               "in the queue. This priority inheritance "
                               "approach is required in "
                               "order to both honor the updated "
                               "LatencyBudget.duration and "
                               "adhere to the DataWriter in-order data "
                               "delivery guarantee."
                               "\n\n"
                               "[default] for DataWriter")
                        .value("HIGHEST_PRIORITY_FIRST",
                               FlowControllerSchedulingPolicy::type::
                                       HIGHEST_PRIORITY_FIRST,
                               "Indicates to flow control in a "
                               "highest-priority-first fashion."
                               "\n\n"
                               "Determines the next destination queue to "
                               "service as determined "
                               "by the publication priority of the DataWriter, "
                               "channel "
                               "of multi-channel DataWriter, or individual "
                               "sample."
                               "\n\n"
                               "The relative priority of a flow controller's "
                               "destination queue "
                               "is determined by the highest publication "
                               "priority of all samples "
                               "it contains."
                               "\n\n"
                               "When tokens become available, the "
                               "FlowController distributes "
                               "tokens to the destination queues in order of "
                               "their publication "
                               "priority. In other words, the queue containing "
                               "the sample with "
                               "the highest publication priority is serviced "
                               "first. The number "
                               "of tokens granted equals the number of tokens "
                               "required to send "
                               "the first sample in the queue. Note that the "
                               "priority of a queue "
                               "may change as samples are sent (i.e. removed "
                               "from the queue). If "
                               "a sample must be sent to multiple destinations "
                               "or two samples "
                               "have an equal publication priority, the "
                               "corresponding "
                               "destination queues are serviced in a "
                               "round-robin fashion."
                               "\n\n"
                               "This priority inheritance approach is required "
                               "in order to both "
                               "honor the designated publication priority and "
                               "adhere to the "
                               "DataWriter in-order data delivery guarantee.")
                        .export_values();
            });

    l.push_back([m]() mutable {
        return init_class<FlowControllerTokenBucketProperty>(
                m,
                "FlowControllerTokenBucketProperty");
    });

    l.push_back([m]() mutable {
        return init_class<FlowControllerProperty>(m, "FlowControllerProperty");
    });

    l.push_back([m]() mutable {
        return init_class<FlowController>(m, "FlowController");
    });
}

}  // namespace pyrti
