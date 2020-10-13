/*
 * (c) 2020 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 *
 * RTI grants Licensee a license to use, modify, compile, and create derivative
 * works of the Software solely for use with RTI products.  The Software is
 * provided "as is", with no warranty of any type, including any warranty for
 * fitness for any purpose. RTI is under no obligation to maintain or support
 * the Software.  RTI shall not be liable for any incidental or consequential
 * damages arising out of the use or inability to use the software.
 */

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
