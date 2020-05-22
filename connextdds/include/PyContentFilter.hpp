#pragma once

#include "PyConnext.hpp"
#include <rti/topic/ContentFilter.hpp>
#include <dds/domain/DomainParticipant.hpp>

namespace py = pybind11;

namespace pyrti {

template<typename T, class CFBase = rti::topic::ContentFilter<T, PyObjectHolder>>
class PyContentFilter : public CFBase {
public:
    using CFBase::CFBase;

    py::object& compile_helper(
        const std::string& expression,
        const std::vector<std::string>& parameters,
        const dds::core::optional<dds::core::xtypes::DynamicType>& type_code,
        const std::string& type_class_name,
        py::object* old_compile_data
    ) {
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

    PyObjectHolder& compile(
        const std::string& expression,
        const std::vector<std::string>& parameters,
        const dds::core::optional<dds::core::xtypes::DynamicType>& type_code,
        const std::string& type_class_name,
        PyObjectHolder* old_compile_data
    ) override {
        py::gil_scoped_acquire acquire;
        py::object none = py::cast(nullptr);
        py::object* old = (nullptr == old_compile_data) ? &none : &old_compile_data->object();
        py::object& compile_data = compile_helper(expression, parameters, type_code, type_class_name, old);
        if (nullptr != old_compile_data) {
            delete old_compile_data;
        }
        return *(new PyObjectHolder(compile_data));
    }

    bool evaluate(
        PyObjectHolder& compile_data,
        const T& sample,
        const rti::topic::FilterSampleInfo& meta_data
    ) override {
        py::gil_scoped_acquire acquire;
        py::object& py_compile_data = compile_data.object();
        PYBIND11_OVERLOAD_PURE(
            bool,
            CFBase,
            evaluate,
            py_compile_data,
            sample,
            meta_data
        );
    }

    void finalize_helper(
        py::object& compile_data
    ) {
        PYBIND11_OVERLOAD_PURE(
            void,
            CFBase,
            finalize,
            compile_data
        );
    }

    void finalize(
        PyObjectHolder& compile_data
    ) override {
        py::gil_scoped_acquire acquire;
        finalize_helper(compile_data.object());
        delete &compile_data;
    }
};

template<typename T>
void init_content_filter_defs(py::class_<
        rti::topic::ContentFilter<T, PyObjectHolder>,
        rti::topic::ContentFilterBase,
        PyContentFilter<T>>& cls) {
    cls
        .def(
            py::init<>()
        )
        .def(
            "compile",
            &rti::topic::ContentFilter<T, PyObjectHolder>::compile,
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
            &rti::topic::ContentFilter<T, PyObjectHolder>::evaluate,
            py::arg("compile_data"),
            py::arg("sample"),
            py::arg("meta_data"),
            "Evaluate whether the sample is passing the filter or not "
            "according to the sample content."
        )
        .def(
            "finalize",
            &rti::topic::ContentFilter<T, PyObjectHolder>::finalize,
            py::arg("compile_data"),
            "A previously compiled instance of the content filter is no "
            "longer in use and resources can now be cleaned up."
        );
}

template<typename T>
void init_content_filter(py::object& o) {
    py::class_<rti::topic::ContentFilter<T, PyObjectHolder>, rti::topic::ContentFilterBase, PyContentFilter<T>> cf(o, "ContentFilter");

    init_content_filter_defs(cf);
}

}