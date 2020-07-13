#include "PyConnext.hpp"
#include "PySeq.hpp"
#include "PyVector.hpp"
#include <rti/core/Cookie.hpp>

using namespace rti::core;

namespace pyrti {

template<>
void init_class_defs(py::class_<Cookie>& cls)
{
    cls.def(py::init<dds::core::vector<uint8_t>&>(), "bytes")
            .def_property_readonly(
                    "value",
                    [](Cookie& c) { return std::vector<uint8_t>(c.value()); },
                    "Retrieve the bytes of the Cookie's contents.")
            .def(py::self == py::self, "Compare Cookies for equality")
            .def(py::self != py::self, "Compare Cookies for inequality")
            .doc() =
            "Unique identifier for a written data sample in the form of a "
            "sequence of bytes.";
}

template<>
void process_inits<Cookie>(py::module& m, ClassInitList& l)
{
    l.push_back([m, &l]() mutable {
        auto func = init_class_with_seq<Cookie>(m, "Cookie");
        init_dds_vector_nonbuffer_class<Cookie>(m, "CookieVector", l);
        return func;
    });
}

}  // namespace pyrti
