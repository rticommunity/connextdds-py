#include "PyConnext.hpp"
#include <rti/core/policy/CorePolicy.hpp>

using namespace dds::core;
using namespace rti::core;
using namespace rti::core::policy;

namespace pyrti {

template <> void init_class_defs(py::class_<Database> &cls) {
  cls.def(py::init<>(), "Create a Database QoS policy with default settings.")
      .def_property(
          "thread", (ThreadSettings & (Database::*)()) & Database::thread,
          (Database & (Database::*)(const ThreadSettings &)) & Database::thread,
          "Get/set the thread settings for the Database thread.")
      .def_property("shutdown_timeout",
                    (Duration(Database::*)() const) &
                        Database::shutdown_timeout,
                    (Database & (Database::*)(const Duration &)) &
                        Database::shutdown_timeout,
                    "Get/set the shutdown timeout.")
      .def_property("cleanup_period",
                    (Duration(Database::*)() const) & Database::cleanup_period,
                    (Database & (Database::*)(const Duration &)) &
                        Database::cleanup_period,
                    "Get/set the cleanup period.")
      .def_property("shutdown_cleanup_period",
                    (Duration(Database::*)() const) &
                        Database::shutdown_cleanup_period,
                    (Database & (Database::*)(const Duration &)) &
                        Database::shutdown_cleanup_period,
                    "Get/set the shutdown cleanup period.")
      .def_property(
          "initial_records",
          (int32_t(Database::*)() const) & Database::initial_records,
          (Database & (Database::*)(int32_t)) & Database::initial_records,
          "Get/set the number of database records to be allocated initially.")
      .def_property(
          "max_skiplist_level",
          (int32_t(Database::*)() const) & Database::max_skiplist_level,
          (Database & (Database::*)(int32_t)) & Database::max_skiplist_level,
          "Get/set the skiplist level.")
      .def_property(
          "max_weak_references",
          (int32_t(Database::*)() const) & Database::max_weak_references,
          (Database & (Database::*)(int32_t)) & Database::max_weak_references,
          "Get/set the maximum number of weak references.")
      .def_property("initial_weak_references",
                    (int32_t(Database::*)() const) &
                        Database::initial_weak_references,
                    (Database & (Database::*)(int32_t)) &
                        Database::initial_weak_references,
                    "Get/set the initial number of weak references.")
      .def(py::self == py::self, "Test for equality.")
      .def(py::self != py::self, "Test for inequality.");
}

template <> void process_inits<Database>(py::module &m, ClassInitList &l) {
  l.push_back([m]() mutable { return init_class<Database>(m, "Database"); });
}

} // namespace pyrti
