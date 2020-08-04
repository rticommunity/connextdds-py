#include "PyConnext.hpp"
#include <rti/pub/WriteParams.hpp>

using namespace rti::pub;

namespace pyrti {

template <> void init_class_defs(py::class_<WriteParams> &cls) {
  cls.def(py::init<>(), "Create a WriteParams object with default values.")
      .def("reset", &WriteParams::reset,
           "Reset all fields to their default values.")
      .def_property("replace_automatic_values",
                    (bool (WriteParams::*)() const) &
                        WriteParams::replace_automatic_values,
                    (WriteParams & (WriteParams::*)(bool)) &
                        WriteParams::replace_automatic_values,
                    "Indicates if the replacement of automatic values has been "
                    "activated or not.")
      .def_property(
          "identity",
          (rti::core::SampleIdentity(WriteParams::*)() const) &
              WriteParams::identity,
          (WriteParams & (WriteParams::*)(const rti::core::SampleIdentity &)) &
              WriteParams::identity,
          "The sample identity.")
      .def_property(
          "related_sample_identity",
          (rti::core::SampleIdentity(WriteParams::*)() const) &
              WriteParams::related_sample_identity,
          (WriteParams & (WriteParams::*)(const rti::core::SampleIdentity &)) &
              WriteParams::related_sample_identity,
          "The related sample identity.")
      .def_property("source_timestamp",
                    (dds::core::Time(WriteParams::*)() const) &
                        WriteParams::source_timestamp,
                    (WriteParams & (WriteParams::*)(const dds::core::Time &)) &
                        WriteParams::source_timestamp,
                    "The source timestamp for writing.")
      .def_property(
          "cookie",
          (rti::core::Cookie(WriteParams::*)() const) & WriteParams::cookie,
          (WriteParams & (WriteParams::*)(const rti::core::Cookie &)) &
              WriteParams::cookie,
          "The cookie for writing.")
      .def_property(
          "handle",
          (dds::core::InstanceHandle(WriteParams::*)() const) &
              WriteParams::handle,
          (WriteParams & (WriteParams::*)(const dds::core::InstanceHandle &)) &
              WriteParams::handle,
          "The instance handle for writing.")
      .def_property(
          "priortiy", (int32_t(WriteParams::*)() const) & WriteParams::priority,
          (WriteParams & (WriteParams::*)(int32_t)) & WriteParams::priority,
          "The priority for writing.")
      .def_property(
          "flag",
          (rti::core::SampleFlag(WriteParams::*)() const) & WriteParams::flag,
          (WriteParams & (WriteParams::*)(const rti::core::SampleFlag &)) &
              WriteParams::flag,
          "The sample flag for writing.")
      .def_property("source_guid",
                    (rti::core::Guid(WriteParams::*)() const) &
                        WriteParams::source_guid,
                    (WriteParams & (WriteParams::*)(const rti::core::Guid &)) &
                        WriteParams::source_guid,
                    "The source GUID for writing.")
      .def_property("related_source_guid",
                    (rti::core::Guid(WriteParams::*)() const) &
                        WriteParams::related_source_guid,
                    (WriteParams & (WriteParams::*)(const rti::core::Guid &)) &
                        WriteParams::related_source_guid,
                    "The related source GUID for writing.")
      .def_property("related_reader_guid",
                    (rti::core::Guid(WriteParams::*)() const) &
                        WriteParams::related_reader_guid,
                    (WriteParams & (WriteParams::*)(const rti::core::Guid &)) &
                        WriteParams::related_reader_guid,
                    "The related reader GUID for writing.");
}

template <> void process_inits<WriteParams>(py::module &m, ClassInitList &l) {
  l.push_back(
      [m]() mutable { return init_class<WriteParams>(m, "WriteParams"); });
}

} // namespace pyrti
