#include "PyConnext.hpp"
#include <dds/sub/SampleInfo.hpp>

using namespace dds::sub;

namespace pyrti {

template<>
void init_class_defs(py::class_<Rank>& cls)
{
    cls.def(py::init<>(), "Create a default Rank object.")
            .def(py::init<int32_t, int32_t, int32_t>(),
                 py::arg("sample_rank"),
                 py::arg("generation_rank"),
                 py::arg("absolute_generation_rank"),
                 "Create a GenerationCount object with the provided "
                 "disposed_count "
                 "and no_writers count.")
            .def_property_readonly(
                    "sample",
                    &Rank::sample,
                    "Get the sample rank of the sample.")
            .def_property_readonly(
                    "generation",
                    &Rank::generation,
                    "Get the generation rank of the sample.")
            .def_property_readonly(
                    "absolute_generation",
                    &Rank::absolute_generation,
                    "Get the absolute generation rank of the sample.");
}

template<>
void process_inits<Rank>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable { return init_class<Rank>(m, "Rank"); });
}

}  // namespace pyrti
