#pragma once
#include <ndds_version.h>

#define rti_connext_version_gt(rti_major, rti_minor, rti_release) \
    ((rti_major < RTI_DDS_VERSION_MAJOR) || \
    (rti_major == RTI_DDS_VERSION_MAJOR && rti_minor < RTI_DDS_VERSION_MINOR) || \
    (rti_major == RTI_DDS_VERSION_MAJOR && rti_minor == RTI_DDS_VERSION_MINOR && rti_release < RTI_DDS_VERSION_RELEASE))

#define rti_connext_version_lt(rti_major, rti_minor, rti_release) \
    ((rti_major > RTI_DDS_VERSION_MAJOR) || \
    (rti_major == RTI_DDS_VERSION_MAJOR && rti_minor > RTI_DDS_VERSION_MINOR) || \
    (rti_major == RTI_DDS_VERSION_MAJOR && rti_minor == RTI_DDS_VERSION_MINOR && rti_release > RTI_DDS_VERSION_RELEASE))

#define rti_connext_version_eq(rti_major, rti_minor, rti_release) \
    (rti_major == RTI_DDS_VERSION_MAJOR && rti_minor == RTI_DDS_VERSION_MINOR && rti_release == RTI_DDS_VERSION_RELEASE)

#define rti_connext_version_gte(rti_major, rti_minor, rti_release) \
    ((rti_connext_version_gt(rti_major, rti_minor, rti_release)) || \
    (rti_connext_version_eq(rti_major, rti_minor, rti_release)))

#define rti_connext_version_lte(rti_major, rti_minor, rti_release) \
    ((rti_connext_version_lt(rti_major, rti_minor, rti_release)) || \
    (rti_connext_version_eq(rti_major, rti_minor, rti_release)))

#include <pybind11/pybind11.h>
#include "PyOpaqueTypes.hpp"
#include <pybind11/operators.h>
#include <list>
#include <dds/core/External.hpp>
#include <dds/core/xtypes/DynamicType.hpp>

namespace py = pybind11;

PYBIND11_DECLARE_HOLDER_TYPE(T, dds::core::external<T>);

namespace pyrti {

typedef std::function<void()> DefInitFunc;
typedef std::function<DefInitFunc()> ClassInitFunc;
typedef std::list<ClassInitFunc> ClassInitList;
typedef std::vector<DefInitFunc> DefInitVector;

template<typename T>
void process_inits(py::module&, ClassInitList&);

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

py::object py_cast_type(dds::core::xtypes::DynamicType&);

class PyObjectHolder {
public:
    PyObjectHolder(py::object& obj) : contained(obj) {}

    py::object& object() {
        return this->contained;
    }

    virtual ~PyObjectHolder() {
        if (!this->contained.is_none()) {
            while (this->contained.ref_count() > 1) {
                this->contained.dec_ref();
            }
        }
    }

private:
    py::object contained;
};

// Dummy classes
class PyVector{};
class PyPrimitiveType{};
class PyBuiltinProfiles{};

}
