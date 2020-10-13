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
using namespace rti::core::xtypes;

namespace pyrti {

template<>
void init_class_defs(
        py::class_<StringType, UnidimensionalCollectionTypeImpl>& cls)
{
    cls.def(py::init<uint32_t>(),
            py::arg("bounds"),
            "Creates a bounded string.")
            .def(py::init([]() {
                    return StringType(UnidimensionalCollectionTypeImpl::UNBOUNDED);
                }))
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.");
}

template<>
void process_inits<StringType>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<StringType, UnidimensionalCollectionTypeImpl>(
                m,
                "StringType");
    });
}

}  // namespace pyrti
