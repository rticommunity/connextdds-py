#pragma once

#include "PyConnext.hpp"
#include <pybind11/stl_bind.h>
#include "PyAnyDataReader.hpp"
#include "PyAnyDataWriter.hpp"
#include "PyAnyTopic.hpp"
#include "PyDataReader.hpp"
#include "PyDataReaderListener.hpp"
#include "PyDataWriter.hpp"
#include "PyDataWriterListener.hpp"
#include "PyTopic.hpp"
#include "PyTopicInstance.hpp"
#include "PyTopicListener.hpp"
#include "PySample.hpp"
#include "PySharedSamples.hpp"
#include "PyLoanedSamples.hpp"
#include "PyValidLoanedSamples.hpp"
#include "PyLoanedSample.hpp"
#include "PyContentFilteredTopic.hpp"
#include "PyContentFilter.hpp"
#include "PyWriterContentFilter.hpp"
#include "PyWriterContentFilterHelper.hpp"

namespace pyrti {
    template<typename T>
    void init_type(py::object& o) {
        py::bind_vector<std::vector<T>>(o, "Seq");
        py::bind_vector<std::vector<std::pair<T, dds::core::Time>>>(o, "TimestampedSeq");

        py::implicitly_convertible<py::iterable, std::vector<T>>();
        py::implicitly_convertible<py::iterable, std::vector<std::pair<T, dds::core::Time>>>();

        pyrti::init_datareader_listener<T>(o);
        pyrti::init_datawriter_listener<T>(o);
        pyrti::init_topic_listener<T>(o);
        pyrti::init_datareader<T>(o);
        pyrti::init_datawriter<T>(o);
        pyrti::init_topic<T>(o);
        pyrti::init_topic_instance<T>(o);
        pyrti::init_sample<T>(o);
        pyrti::init_shared_samples<T>(o);
        pyrti::init_loaned_samples<T>(o);
        pyrti::init_valid_loaned_samples<T>(o);
        pyrti::init_loaned_sample<T>(o);
        pyrti::init_content_filtered_topic<T>(o);
        pyrti::init_content_filter<T>(o);
        pyrti::init_writer_content_filter<T>(o);
        pyrti::init_writer_content_filter_helper<T>(o);
    }
}