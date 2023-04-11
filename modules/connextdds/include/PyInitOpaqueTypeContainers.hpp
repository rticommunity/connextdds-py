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

#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl_bind.h>

#define INIT_OPAQUE_TYPE_CONTAINERS(topic_type)                                \
    PYBIND11_MAKE_OPAQUE(std::vector<topic_type>);                             \
    PYBIND11_MAKE_OPAQUE(std::vector<std::pair<topic_type, dds::core::Time>>); \
    PYBIND11_MAKE_OPAQUE(std::vector<pyrti::PyDataWriter<topic_type>>);        \
    PYBIND11_MAKE_OPAQUE(std::vector<pyrti::PyDataReader<topic_type>>);        \
    PYBIND11_MAKE_OPAQUE(std::vector<pyrti::PyTopic<topic_type>>);             \
    PYBIND11_MAKE_OPAQUE(std::vector<pyrti::PyContentFilteredTopic<topic_type>>)
