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

using namespace rti::core::xtypes;
using namespace dds::core::xtypes;

namespace pyrti {

template<>
void init_class_defs(
        py::class_<UnidimensionalCollectionTypeImpl, CollectionType>& cls)
{
    cls.def_property_readonly(
               "bounds",
               &UnidimensionalCollectionTypeImpl::bounds,
               "Gets the maximum length of this collection.")
            .def_property_readonly_static(
                "UNBOUNDED",
                [](py::object&) { return UnidimensionalCollectionTypeImpl::UNBOUNDED; },
                "Constant for specifying an unbounded sequence.")
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.");
}

template<>
void process_inits<UnidimensionalCollectionTypeImpl>(
        py::module& m,
        ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<UnidimensionalCollectionTypeImpl, CollectionType>(
                m,
                "UnidimensionalCollectionType");
    });
}

}  // namespace pyrti
