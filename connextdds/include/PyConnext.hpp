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
#include <dds/core/Optional.hpp>
#include <rti/core/OptionalValue.hpp>
#include <dds/core/xtypes/DynamicType.hpp>

namespace py = pybind11;

#ifdef _MSVC_VER
#define _SYMBOL_HIDDEN
#else
#define _SYMBOL_HIDDEN __attribute__((visibility("hidden")))
#endif

PYBIND11_DECLARE_HOLDER_TYPE(T, dds::core::external<T>);

namespace pyrti {

typedef std::function<void()> DefInitFunc;
typedef std::function<DefInitFunc()> ClassInitFunc;
typedef std::list<ClassInitFunc> ClassInitList;
typedef std::vector<DefInitFunc> DefInitVector;

template<typename T>
void process_inits(py::module&, ClassInitList&);

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

// Dummy classes
class PyPrimitiveType{};
class PyBuiltinProfiles{};

}

namespace pybind11 { namespace detail {
template <typename T> 
struct type_caster<dds::core::optional<T*>> {
    using base = type_caster_base<T>;

public:
    PYBIND11_TYPE_CASTER(dds::core::optional<T*>, _("Optional[") + base::name + _("]"));

    bool load(handle src, bool convert) {
        base base_caster;
        if (src.is_none()) {
            value.reset();
            return true;
        }
        else if (base_caster.load(src, convert)) {
            value = static_cast<T*>(base_caster);
            return true;
        }
        return false;
    }

    static handle cast(const dds::core::optional<T*> &src, return_value_policy r, handle h) {
#if rti_connext_version_gte(6, 0, 1)
        if (src.has_value()) return base::cast(src.value(), r, h);
#else
        if (src.is_set()) return base::cast(src.get(), r, h);
#endif
        return py::none().release();
    }
};
}}

namespace pybind11 { namespace detail {
template <typename T> 
struct type_caster<dds::core::optional<T>> {
    using base = type_caster<T>;

public:
    PYBIND11_TYPE_CASTER(dds::core::optional<T>, _("Optional[") + base::name + _("]"));

    bool load(handle src, bool convert) {
        base base_caster;
        if (src.is_none()) {
            value.reset();
            return true;
        }
        else if (base_caster.load(src, convert)) {
            value = static_cast<T&>(base_caster);
            return true;
        }
        return false;
    }

    static handle cast(const dds::core::optional<T> &src, return_value_policy r, handle h) {
#if rti_connext_version_gte(6, 0, 1)
        if (src.has_value()) return base::cast(src.value(), r, h);
#else
        if (src.is_set()) return base::cast(src.get(), r, h);
#endif
        return py::none().release();
    }
};
}}

namespace pybind11 { namespace detail {
template <typename T> 
struct type_caster<rti::core::optional_value<T>> {
    using base = type_caster<T>;
public:
    PYBIND11_TYPE_CASTER(rti::core::optional_value<T>, _("Optional[") + base::name + _("]"));

    bool load(handle src, bool convert) {
        base base_caster;
        if (src.is_none()) {
            value.reset();
            return true;
        }
        else if (base_caster.load(src, convert)) {
            value = rti::core::optional_value<T>(static_cast<T&>(base_caster));
            return true;
        }
        return false;
    }

    static handle cast(const rti::core::optional_value<T> &src, return_value_policy r, handle h) {
#if rti_connext_version_gte(6, 0, 1)
        if (src.has_value()) return base::cast(src.value(), r, h);
#else
        if (src.is_set()) return base::cast(src.get(), r, h);
#endif
        return py::none().release();
    }
};
}}
