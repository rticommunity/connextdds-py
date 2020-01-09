#pragma once

#include <pybind11/pybind11.h>

#define INIT_OPAQUE_TYPE_CONTAINERS(topic_type) \
    PYBIND11_MAKE_OPAQUE(std::vector<topic_type>); \
    PYBIND11_MAKE_OPAQUE(std::vector<std::pair<topic_type, dds::core::Time>>); \
    PYBIND11_MAKE_OPAQUE(std::vector<dds::topic::TopicInstance<topic_type>>); \
    PYBIND11_MAKE_OPAQUE(std::vector<std::pair<dds::topic::TopicInstance<topic_type>, dds::core::Time>>); \
    PYBIND11_MAKE_OPAQUE(std::vector<pyrti::PyDataWriter<topic_type>>); \
    PYBIND11_MAKE_OPAQUE(std::vector<pyrti::PyDataReader<topic_type>>); \
    PYBIND11_MAKE_OPAQUE(std::vector<pyrti::PyTopic<topic_type>>); \
    PYBIND11_MAKE_OPAQUE(std::vector<pyrti::PyContentFilteredTopic<topic_type>>)

