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
#include <dds/core/xtypes/CollectionTypes.hpp>

using namespace dds::core::xtypes;

namespace pyrti {

template<>
void init_class_defs(py::class_<ArrayType, CollectionType>& cls)
{
    cls.def(py::init<const DynamicType&, uint32_t>(),
            py::arg("data_type"),
            py::arg("dimension"),
            "Creates a unidimensional array.")
            .def(py::init([](const DynamicType& dt,
                             const std::vector<int32_t>& v) {
                     return ArrayType(dt, v.begin(), v.end());
                 }),
                 py::arg("data_type"),
                 py::arg("dimensions"),
                 "Create a multidimensional array.")
            .def_property_readonly(
                    "dimension_count",
                    &ArrayType::dimension_count,
                    "Number of dimensions.")
            .def(
                    "dimension",
                    [](const ArrayType& at, uint32_t index) {
                        if (index >= at.dimension_count())
                            throw py::index_error();
                        return at.dimension(index);
                    },
                    py::arg("index"),
                    "Returns the size of the ith dimension")
            .def_property_readonly(
                    "total_element_count",
                    &ArrayType::total_element_count,
                    "Total element count across all dimensions.")
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.");
}

template<>
void process_inits<ArrayType>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<ArrayType, CollectionType>(m, "ArrayType");
    });
}

}  // namespace pyrti
