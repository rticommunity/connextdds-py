#include "PyConnext.hpp"
#include <rti/core/Guid.hpp>

using namespace rti::core;

namespace pyrti {

template<>
void init_class_defs(py::class_<Guid>& cls) {
    cls
        .def(
            py::init<>(),
            "Create a default Guid (equivalent to unknown Guid)."
        )
        .def(
            "__getitem__",
            [](Guid& g, uint32_t index) {
                return g[index];
            }
        )
        .def(
            "__setitem__",
            [](Guid& g, uint32_t index, uint8_t value) {
                return g[index] = value;
            }
        )
        .def(
            py::self == py::self,
            "Compare Guids for equality."
        )
        .def(
            py::self != py::self,
            "Compare Guids for inequality."
        )
        .def(
            py::self < py::self,
            "Compare Guids."
        )
        .def(
            py::self <= py::self,
            "Compare Guids."
        )
        .def(
            py::self > py::self,
            "Compare Guids."
        )
        .def(
            py::self >= py::self,
            "Compare Guids."
        )
        .def(
            "__len__",
            [](Guid& g) {
                return g.LENGTH;
            }
        )
        .def(
            "__str__",
            [](Guid& g) {
                std::ostringstream oss;
                oss << g;
                return oss.str();
            }
        )
        .def_static(
            "unknown",
            &Guid::unknown,
            "Create an unknown Guid."
        )
        .def_static(
            "automatic",
            &Guid::automatic,
            "Construct a Guid indicating that RTI Connext should choose an "
            "appropriate virtual GUID."
        );
}

template<>
void process_inits<Guid>(py::module& m, ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return init_class<Guid>(m, "Guid");
        }
    );
}

}
