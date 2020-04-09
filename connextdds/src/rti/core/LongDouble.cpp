#include "PyConnext.hpp"
#include <sstream>
#include <rti/core/LongDouble.hpp>

using namespace rti::core;

namespace pyrti {

#if (RTI_CDR_SIZEOF_LONG_DOUBLE == 16)
#else

template<>
void init_class_defs(py::class_<LongDouble>& cls) {
     cls
        .def(
            py::init<>(),
            "Creates a LongDouble with value 0."
        )
        .def(
            py::init(
                [](std::vector<uint8_t>& bytes) {
                    if (bytes.size() != 16) {
                        throw py::type_error("Size mismatch for LongDouble");
                    }
                    LongDouble ld;
                    auto buf = reinterpret_cast<uint8_t*>(&ld.native().bytes);
                    std::copy(bytes.begin(), bytes.end(), buf);
                    return ld;
                }
            ),
            py::arg("byte_sequence"),
            "Creates a LongDouble from a sequence of bytes."
        )
        .def(
            "__getitem__",
            [](const LongDouble& ld, size_t index) {
                return static_cast<uint8_t>(ld[index]);
            },
            "Get a byte from the LongDouble buffer."
        )
        .def(
            "__setitem__",
            [](LongDouble& ld, size_t index, uint8_t b) {
                ld[index] = static_cast<char>(b);
            },
            "Set a byte in the LongDouble buffer."
        )
        .def(
            "__str__",
            [](const LongDouble& ld) {
                std::ostringstream oss;
                oss << ld;
                return oss.str();
            }
        )
        .def(
            py::self == py::self,
            "Test for equality"
        )
        .def(
            py::self != py::self,
            "Test for inequality"
        );

    py::implicitly_convertible<std::vector<uint8_t>, LongDouble>();
}

#endif

template<>
void process_inits<LongDouble>(py::module& m, ClassInitList& l) {
#if (RTI_CDR_SIZEOF_LONG_DOUBLE == 16)
#else
    l.push_back(
        [m]() mutable {
            return init_class<LongDouble>(m, "LongDouble");
        }
    );
#endif
}

}
