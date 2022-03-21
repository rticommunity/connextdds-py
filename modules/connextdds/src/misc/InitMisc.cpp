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
#include "PyVector.hpp"
#include "PyNamespaces.hpp"

namespace py = pybind11;

void init_constants(py::module&);
void init_dds_stl_bindings(py::module&);

// Init all bindings that don't depend on DDS for signatures
void init_misc_early(py::module& m, pyrti::ClassInitList& l)
{
    init_constants(m);
    init_dds_stl_bindings(m);
    pyrti::init_dds_vector_buffer_class<uint8_t>(m, "ByteVector", l);
    pyrti::init_dds_vector_buffer_class<int32_t>(m, "Int32Vector", l);
}

// Init all bindings that depend on DDS for signatures
void init_misc_late(py::module&)
{
}