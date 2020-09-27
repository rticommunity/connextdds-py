#include "PyConnext.hpp"
#include <rti/sub/AckResponseData.hpp>

using namespace rti::sub;

namespace pyrti {

template<>
void init_class_defs(py::class_<AckResponseData>& cls)
{
    cls.def(py::init<>(), "Create an empty ack response.")
            .def(py::init<std::vector<uint8_t>>(),
                 py::arg("sequence"),
                 "Create an instance with a byte sequence.")
            .def_property(
                    "value",
                    (std::vector<uint8_t>(AckResponseData::*)() const)
                            & AckResponseData::value,
                    (std::vector<
                             uint8_t> & (AckResponseData::*) (std::vector<uint8_t>&) const)
                            & AckResponseData::value,
                    "Gets the subscription handle of the acknowledging "
                    "DataReade");
}


template<>
void process_inits<AckResponseData>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<AckResponseData>(m, "AckResponseData");
    });
}

}  // namespace pyrti
