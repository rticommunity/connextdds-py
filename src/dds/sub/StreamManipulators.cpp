#include "PyConnext.hpp"
#include <dds/sub/DataReader.hpp>

template<>
void pyrti::process_inits<pyrti::PyStreamManipulators>(py::module& m, pyrti::ClassInitList& l) {
    m.def_submodule("stream", "A module for stream manipulators")
        .def(
            "read",
            &dds::sub::read,
            "DataReader stream manipulator for reading samples."
        )
        .def(
            "take",
            &dds::sub::take,
            "DataReader stream manipulator for taking samples."
        )
        .def(
            "max_samples",
            &dds::sub::max_samples,
            py::arg("count"),
            "DataReader stream manipulator for specifying max samples."
        )
        .def(
            "content",
            &dds::sub::content,
            py::arg("query_condition"),
            "DataReader stream manipulator for specifying content query."
        )
        .def(
            "condition",
            &dds::sub::condition,
            py::arg("read_condition"),
            "DataReader stream manipulator for specifying read condition."
        )
        .def(
            "state",
            &dds::sub::state,
            py::arg("data_state"),
            "DataReader stream manipulator for specifying data state filter."
        )
        .def(
            "instance",
            &dds::sub::instance,
            py::arg("handle"),
            "DataReader stream manipulator for specifying an instance."
        )
        .def(
            "next_instance",
            &dds::sub::next_instance,
            py::arg("handle"),
            "DataReader stream manipulator for specifying an instance after "
            "the provided handle."
        );

    py::class_<dds::sub::ReadModeDummyType>(m, "ReadModeDummy");

    py::class_<dds::sub::functors::MaxSamplesManipulatorFunctor>(m, "MaxSamplesManipulator");

    py::class_<dds::sub::functors::ContentFilterManipulatorFunctor>(m, "ContentFilterManipulator");

    py::class_<dds::sub::functors::ConditionManipulatorFunctor>(m, "ConditionManipulator");

    py::class_<dds::sub::functors::StateFilterManipulatorFunctor>(m, "StateFilterManipulator");

    py::class_<dds::sub::functors::InstanceManipulatorFunctor>(m, "InstanceManipulator");

    py::class_<dds::sub::functors::NextInstanceManipulatorFunctor>(m, "NextInstanceManipulator");
}
