#include "PyConnext.hpp"
#include <dds/core/policy/CorePolicy.hpp>

using namespace dds::core::policy;

namespace pyrti {

template<>
void init_class_defs<Partition>(py::class_<Partition>& cls)
{
    cls.def(py::init<>(), "Creates a policy with the default partition.")
            .def(py::init<const std::string&>(),
                 py::arg("partition"),
                 "Creates a policy with a single partition with the specified "
                 "name.")
            .def(py::init<const std::vector<std::string>&>(),
                 py::arg("partitions"),
                 "Creates a policy with the partitions specified in a "
                 "sequence.")
            .def_property(
                    "name",
                    (const std::vector<std::string> (Partition::*)() const)
                            & Partition::name,
                    (Partition
                     & (Partition::*) (const std::vector<std::string>&) )
                            & Partition::name,
                    "The partition names specified in a sequence.")
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.");
}

template<>
void process_inits<Partition>(py::module& m, ClassInitList& l)
{
    l.push_back(
            [m]() mutable { return init_class<Partition>(m, "Partition"); });
}

}  // namespace pyrti
