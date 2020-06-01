#pragma once

#include "PyConnext.hpp"
#include "PyContentFilter.hpp"

namespace pyrti {

template<typename T, class WCFBase = rti::topic::WriterContentFilter<T, dds::core::optional<py::object>, dds::core::optional<py::object>>>
class PyWriterContentFilter : public PyContentFilter<T, WCFBase> {
public:
    using PyContentFilter<T, WCFBase>::PyContentFilter;

    void writer_compile(
        dds::core::optional<py::object>& writer_filter_data,
        rti::topic::ExpressionProperty& prop,
        const std::string& expression,
        const std::vector<std::string>& parameters,
        const dds::core::optional<dds::core::xtypes::DynamicType>& type_code,
        const std::string& type_class_name,
        const rti::core::Cookie& cookie
    ) override {
        PYBIND11_OVERLOAD_PURE(
            void,
            WCFBase,
            writer_compile,
            writer_filter_data,
            &prop,
            expression,
            parameters,
            type_code,
            type_class_name,
            cookie
        );
    }

    dds::core::vector<rti::core::Cookie>& writer_evaluate(
        dds::core::optional<py::object>& writer_filter_data,
        const T& sample,
        const rti::topic::FilterSampleInfo& meta_data
    ) override {
        PYBIND11_OVERLOAD_PURE(
            dds::core::vector<rti::core::Cookie>&,
            WCFBase,
            writer_evaluate,
            writer_filter_data,
            sample,
            meta_data
        );
    }

    void writer_finalize(
        dds::core::optional<py::object>& writer_filter_data,
        const rti::core::Cookie& cookie
    ) override {
        PYBIND11_OVERLOAD_PURE(
            void,
            WCFBase,
            writer_finalize,
            writer_filter_data,
            cookie
        );
    }

    dds::core::optional<py::object>& writer_attach_helper() {
        PYBIND11_OVERLOAD_PURE(
            dds::core::optional<py::object>&,
            WCFBase,
            writer_attach,
        );
    }

    dds::core::optional<py::object>& writer_attach() override {
        dds::core::optional<py::object>& writer_data = writer_attach_helper();
#if rti_connext_version_gte(6, 0, 1)
        if (writer_data.has_value()) writer_data.value().inc_ref();
#else
        if (writer_data.is_set()) writer_data.get().inc_ref();
#endif
        return *(new dds::core::optional<py::object>(writer_data));
    }

    void writer_detach_helper(
        dds::core::optional<py::object>& writer_filter_data
    ) {
        PYBIND11_OVERLOAD_PURE(
            void,
            WCFBase,
            writer_detach,
            writer_filter_data
        );
    }

    void writer_detach(
        dds::core::optional<py::object>& writer_filter_data
    ) override {
        writer_detach_helper(writer_filter_data);
        delete &writer_filter_data;
    }

    void writer_return_loan(
        dds::core::optional<py::object>& writer_filter_data,
        dds::core::vector<rti::core::Cookie>& cookies
    ) override {
        PYBIND11_OVERLOAD_PURE(
            void,
            WCFBase,
            writer_return_loan,
            writer_filter_data,
            cookies
        );
    }
};

template<typename T>
void init_writer_content_filter_defs(py::class_<
        rti::topic::WriterContentFilter<T, dds::core::optional<py::object>, dds::core::optional<py::object>>,
        rti::topic::ContentFilter<T, dds::core::optional<py::object>>, 
        PyWriterContentFilter<T>>& cls) {
    cls
        .def(
            py::init<>()
        )
        .def(
            "writer_compile",
            &rti::topic::WriterContentFilter<T, dds::core::optional<py::object>, dds::core::optional<py::object>>::writer_compile,
            py::arg("writer_filter_data"),
            py::arg("property"),
            py::arg("expression"),
            py::arg("parameters"),
            py::arg("type_code"),
            py::arg("type_class_name"),
            py::arg("cookie"),
            "A writer-side filtering API to compile an instance of the "
            "content filter according to the filter expression and "
            "parameters specified by a matching DataReader."
        )
        .def(
            "writer_evaluate",
            &rti::topic::WriterContentFilter<T, dds::core::optional<py::object>, dds::core::optional<py::object>>::writer_evaluate,
            py::arg("writer_filter_data"),
            py::arg("sample"),
            py::arg("meta_data"),
            "A writer-side filtering API to compile an instance of the "
            "content filter according to the filter expression and "
            "parameters specified by a matching DataReader."
        )
        .def(
            "writer_finalize",
            &rti::topic::WriterContentFilter<T, dds::core::optional<py::object>, dds::core::optional<py::object>>::writer_finalize,
            py::arg("writer_filter_data"),
            py::arg("cookie"),
            "A writer-side filtering API to clean up a previously "
            "compiled instance of the content filter."
        )
        .def(
            "writer_attach",
            &rti::topic::WriterContentFilter<T, dds::core::optional<py::object>, dds::core::optional<py::object>>::writer_attach,
            "A writer-side filtering API to create some state that can "
            "facilitate filtering on the writer side."
        )
        .def(
            "writer_detach",
            &rti::topic::WriterContentFilter<T, dds::core::optional<py::object>, dds::core::optional<py::object>>::writer_detach,
            py::arg("writer_filter_data"),
            "A writer-side filtering API to clean up a previously created "
            "state using writer_attach."
        )
        .def(
            "writer_return_loan",
            &rti::topic::WriterContentFilter<T, dds::core::optional<py::object>, dds::core::optional<py::object>>::writer_return_loan,
            py::arg("writer_filter_data"),
            py::arg("cookies"),
            "A writer-side filtering API to return the loan on the list "
            "of DataReaders returned by writer_evaluate."
        );
}

template<typename T>
void init_writer_content_filter(
    py::class_<rti::topic::WriterContentFilter<T, dds::core::optional<py::object>, dds::core::optional<py::object>>, rti::topic::ContentFilter<T, dds::core::optional<py::object>>, PyWriterContentFilter<T>>& wcf
) {
    init_writer_content_filter_defs(wcf);
}

}