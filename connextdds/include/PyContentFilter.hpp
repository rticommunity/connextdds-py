#pragma once

#include "PyConnext.hpp"
#include <rti/topic/ContentFilter.hpp>
#include <dds/domain/DomainParticipant.hpp>

namespace py = pybind11;

namespace pyrti {

template<typename T, class CFBase = rti::topic::ContentFilter<T, dds::core::optional<py::object>>>
class PyContentFilter : public CFBase {
public:
    using CFBase::CFBase;

    dds::core::optional<py::object>& compile_helper(
        const std::string& expression,
        const std::vector<std::string>& parameters,
        const dds::core::optional<dds::core::xtypes::DynamicType>& type_code,
        const std::string& type_class_name,
        dds::core::optional<py::object>* old_compile_data
    ) {
        PYBIND11_OVERLOAD_PURE(
            dds::core::optional<py::object>&,
            CFBase,
            compile,
            expression,
            parameters,
            type_code,
            type_class_name,
            old_compile_data
        );
    }

    dds::core::optional<py::object>& compile(
        const std::string& expression,
        const std::vector<std::string>& parameters,
        const dds::core::optional<dds::core::xtypes::DynamicType>& type_code,
        const std::string& type_class_name,
        dds::core::optional<py::object>* old_compile_data
    ) override {
        auto compile_data = compile_helper(expression, parameters, type_code, type_class_name, old_compile_data);
        if (nullptr != old_compile_data) {
            delete old_compile_data;
        }
        if (has_value(compile_data)) get_value(compile_data).inc_ref();
        return *(new dds::core::optional<py::object>(compile_data));
    }

    bool evaluate(
        dds::core::optional<py::object>& compile_data,
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

    void finalize_helper(
        dds::core::optional<py::object>& compile_data
    ) {
        PYBIND11_OVERLOAD_PURE(
            void,
            CFBase,
            finalize,
            compile_data
        );
    }

    void finalize(
        dds::core::optional<py::object>& compile_data
    ) override {
        finalize_helper(compile_data);
        delete &compile_data;
    }
};

template<typename T>
void init_content_filter_defs(py::class_<
        rti::topic::ContentFilter<T, dds::core::optional<py::object>>,
        rti::topic::ContentFilterBase,
        PyContentFilter<T>>& cls) {
    cls
        .def(
            py::init<>()
        )
        .def(
            "compile",
            &rti::topic::ContentFilter<T, dds::core::optional<py::object>>::compile,
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
            &rti::topic::ContentFilter<T, dds::core::optional<py::object>>::evaluate,
            py::arg("compile_data"),
            py::arg("sample"),
            py::arg("meta_data"),
            "Evaluate whether the sample is passing the filter or not "
            "according to the sample content."
        )
        .def(
            "finalize",
            &rti::topic::ContentFilter<T, dds::core::optional<py::object>>::finalize,
            py::arg("compile_data"),
            "A previously compiled instance of the content filter is no "
            "longer in use and resources can now be cleaned up."
        );
}

template<typename T>
void init_content_filter(
    py::class_<rti::topic::ContentFilter<T, dds::core::optional<py::object>>, rti::topic::ContentFilterBase, PyContentFilter<T>>& cf
) {
    init_content_filter_defs(cf);
}

}