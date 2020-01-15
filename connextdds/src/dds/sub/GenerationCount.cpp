#include "PyConnext.hpp"
#include <dds/sub/SampleInfo.hpp>

using namespace dds::sub;

template<>
void pyrti::init_class_defs(py::class_<GenerationCount>& cls) {
    cls
        .def(
            py::init<>(),
            "Create a default GenerationCount object."
        )
        .def(
            py::init<
                int32_t,
                int32_t
            >(),
            py::arg("disposed_count"),
            py::arg("no_writers_count"),
            "Create a GenerationCount object with the provided disposed_count "
            "and no_writers count."
        )
        .def_property_readonly(
            "disposed",
            &GenerationCount::disposed,
            "Get the disposed generation count."
        )
        .def_property_readonly(
            "no_writers",
            &GenerationCount::no_writers,
            "Get the no_writers generation count."
        );
}

template<>
void pyrti::process_inits<GenerationCount>(py::module& m, pyrti::ClassInitList& l) {
    l.push_back(
        [m]() mutable{
            return pyrti::init_class<GenerationCount>(m, "GenerationCount");
        }
    );
}