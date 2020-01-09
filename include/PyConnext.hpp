#pragma once
#include <pybind11/pybind11.h>
#include "PyOpaqueTypes.hpp"
#include <pybind11/operators.h>
#include <list>

namespace py = pybind11;

namespace pyrti {
    typedef std::function<void()> DefInitFunc;
    typedef std::function<DefInitFunc()> ClassInitFunc;
    typedef std::list<ClassInitFunc> ClassInitList;
    typedef std::vector<DefInitFunc> DefInitVector;

    template<typename T>
    void process_inits(py::module&, pyrti::ClassInitList&);

    /*template<typename T>
    void process_inits(py::object&) {
        throw std::runtime_error("Template specialization undefined for this type.");
    }*/

    template<typename T, typename... Bases>
    void init_class_defs(py::class_<T, Bases...>& cls);

    template<typename T, typename... Bases>
    DefInitFunc init_class(py::object& parent, const std::string& cls_name) {
        py::class_<T, Bases...> cls(parent, cls_name.c_str());
        return (
            [cls]() mutable {
                init_class_defs<T>(cls);
            }     
        );
    }

    template<typename from_class, typename to_class, typename py_to_class>
    void add_conversion(py_to_class& cls, const std::string& doc = "Convert to less-specific type.") {
        cls
            .def(
                py::init<
                    const from_class&
                >(),
                py::arg("value"),
                doc.c_str()
            );

        py::implicitly_convertible<from_class, to_class>();
    }

    template<typename from_class, typename to_class, typename py_to_class>
    void add_conversion(py_to_class&& cls, const std::string& doc = "Convert to less-specific type.") {
        cls
            .def(
                py::init<
                    const from_class&
                >(),
                py::arg("value"),
                doc.c_str()
            );

        py::implicitly_convertible<from_class, to_class>();
    }

    // Dummy classes
    class PyVector{};
    class PyPrimitiveType{};
    class PyBuiltinTopicTypes{};
    class PyStreamManipulators{};
}
