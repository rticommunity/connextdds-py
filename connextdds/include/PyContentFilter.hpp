#pragma once

#include "PyConnext.hpp"
#include <rti/topic/ContentFilter.hpp>
#include <dds/domain/DomainParticipant.hpp>

namespace py = pybind11;

namespace pyrti {

template<typename T, class CFBase = rti::topic::ContentFilter<T, py::object>>
class PyContentFilter : public CFBase {
public:
    using CFBase::CFBase;

    py::object& compile(
        const std::string& expression,
        const std::vector<std::string>& parameters,
        const dds::core::optional<dds::core::xtypes::DynamicType>& type_code,
        const std::string& type_class_name,
        py::object* old_compile_data
    ) override {
        auto py_type_code = type_code.is_set() ? py::cast(type_code.get()) : py::cast(nullptr);
        PYBIND11_OVERLOAD_PURE(
            py::object&,
            CFBase,
            compile,
            expression,
            parameters,
            py_type_code,
            type_class_name,
            old_compile_data
        );
    }

    bool evaluate(
        py::object& compile_data,
        const T& sample,
        const rti::topic::FilterSampleInfo& meta_data
    ) override {
        PYBIND11_OVERLOAD_PURE(
            bool,
            CFBase,
            evaluate,
            compile_data,
            sample,
            meta_data
        );
    }

    void finalize(
        py::object& compile_data
    ) override {
        PYBIND11_OVERLOAD_PURE(
            void,
            CFBase,
            finalize,
            compile_data
        );
    }
};

template<typename T>
void init_content_filter_defs(py::class_<rti::topic::ContentFilter<T, py::object>, PyContentFilter<T>>& cls) {
    cls
        .def(
            "compile",
            &rti::topic::ContentFilter<T, py::object>::compile,
            py::arg("expression"),
            py::arg("parameters"),
            py::arg("type_code"),
            py::arg("type_class_name"),
            py::arg("old_compile_data"),
            "Compile an instance of the content filter according to the "
            "filter expression and parameters of the given data type."
        )
        .def(
            "evaluate",
            &rti::topic::ContentFilter<T, py::object>::evaluate,
            py::arg("compile_data"),
            py::arg("sample"),
            py::arg("meta_data"),
            "Evaluate whether the sample is passing the filter or not "
            "according to the sample content."
        )
        .def(
            "finalize",
            &rti::topic::ContentFilter<T, py::object>::finalize,
            py::arg("compile_data"),
            "A previously compiled instance of the content filter is no "
            "longer in use and resources can now be cleaned up."
        )
        .def_static(
            "lookup",
            [](const PyDomainParticipant& dp, const std::string& name) {
                return rti::topic::find_content_filter<T>(dp, name);
            },
            "Lookup a content filter previously registered with "
            "DomainParticipant.register_contentfilter."
            "\n\n"
            "You cannot lookup the RTI Connext built-in content filters."
        );
}

template<typename T>
void init_content_filter(py::object& o) {
    py::class_<rti::topic::ContentFilter<T, py::object>, PyContentFilter<T>> cf(o, "ContentFilter");

    init_content_filter_defs(cf);
}

}