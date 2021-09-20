/*
 * (c) 2021 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 *
 * RTI grants Licensee a license to use, modify, compile, and create derivative
 * works of the Software solely for use with RTI products.  The Software is
 * provided "as is", with no warranty of any type, including any warranty for
 * fitness for any purpose. RTI is under no obligation to maintain or support
 * the Software.  RTI shall not be liable for any incidental or consequential
 * damages arising out of the use or inability to use the software.
 */


#include "PyConnext.hpp"
#include "PyCondition.hpp"
#include <rti/request/detail/Common.hpp>
#include <rti/request/detail/RequesterImpl.hpp>
namespace pyrti {

pyrti::PyReadCondition create_correlation_condition(
    pyrti::PyIAnyDataReader& reader,
    dds::sub::status::SampleState& state,
    rti::core::SequenceNumber& sn) 
{
    return PyReadCondition(rti::request::detail::create_correlation_condition(
        reader.get_any_datareader(),
        state,
        sn));
}


void create_correlation_index(pyrti::PyIAnyDataReader& reader)
{
    rti::request::detail::create_correlation_index(reader.get_any_datareader()->native_reader());
}

}


PYBIND11_MODULE(_util_native, m) {
    py::module::import("rti.connextdds");

    m.def(
        "create_correlation_condition",
        &pyrti::create_correlation_condition,
        py::arg("reader"),
        py::arg("sample_state"),
        py::arg("related_sample_sn")
    );

    m.def(
        "create_correlation_index",
        &pyrti::create_correlation_index,
        py::arg("reader")
    );

}