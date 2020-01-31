#include "PyConnext.hpp"
#include <dds/core/xtypes/DynamicData.hpp>

using namespace rti::core::xtypes;

namespace pyrti {

template<>
void init_class_defs(py::class_<LoanedDynamicData>& cls) {
    cls
        .def(
            "return_loan",
            &LoanedDynamicData::return_loan,
            "Explicitly return a dynamic data loan."
        )
        .def(
            "get",
            (dds::core::xtypes::DynamicData& (LoanedDynamicData::*)()) &LoanedDynamicData::get, 
            "Obtains the DynamicData object representing a member of a DynamicData object."
        )
        .def(
            "get",
            (const dds::core::xtypes::DynamicData& (LoanedDynamicData::*)() const) &LoanedDynamicData::get,
            "Obtains the DynamicData object representing a member of a DynamicData object."
        );
}

template<>
void process_inits<LoanedDynamicData>(py::module& m, ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return init_class<LoanedDynamicData>(m, "LoanedDynamicData");
        }
    );
}

}
