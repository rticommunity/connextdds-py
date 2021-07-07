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
#include <ndds_version.h>
#include <dds/dds.hpp>

#ifdef _MSC_VER
    #define PYRTI_SYMBOL_HIDDEN
    #define PYRTI_SYMBOL_PUBLIC __declspec(dllexport)
#else
    #define PYRTI_SYMBOL_HIDDEN __attribute__((visibility("hidden")))
    #define PYRTI_SYMBOL_PUBLIC __attribute__((visibility("default")))
#endif

#define rti_connext_version_gt(rti_major, rti_minor, rti_release) \
    ((rti_major < RTI_DDS_VERSION_MAJOR)                          \
     || (rti_major == RTI_DDS_VERSION_MAJOR                       \
         && rti_minor < RTI_DDS_VERSION_MINOR)                    \
     || (rti_major == RTI_DDS_VERSION_MAJOR                       \
         && rti_minor == RTI_DDS_VERSION_MINOR                    \
         && rti_release < RTI_DDS_VERSION_RELEASE))

#define rti_connext_version_lt(rti_major, rti_minor, rti_release) \
    ((rti_major > RTI_DDS_VERSION_MAJOR)                          \
     || (rti_major == RTI_DDS_VERSION_MAJOR                       \
         && rti_minor > RTI_DDS_VERSION_MINOR)                    \
     || (rti_major == RTI_DDS_VERSION_MAJOR                       \
         && rti_minor == RTI_DDS_VERSION_MINOR                    \
         && rti_release > RTI_DDS_VERSION_RELEASE))

#define rti_connext_version_eq(rti_major, rti_minor, rti_release)             \
    (rti_major == RTI_DDS_VERSION_MAJOR && rti_minor == RTI_DDS_VERSION_MINOR \
     && rti_release == RTI_DDS_VERSION_RELEASE)

#define rti_connext_version_gte(rti_major, rti_minor, rti_release) \
    ((rti_connext_version_gt(rti_major, rti_minor, rti_release))   \
     || (rti_connext_version_eq(rti_major, rti_minor, rti_release)))

#define rti_connext_version_lte(rti_major, rti_minor, rti_release) \
    ((rti_connext_version_lt(rti_major, rti_minor, rti_release))   \
     || (rti_connext_version_eq(rti_major, rti_minor, rti_release)))

#include <pybind11/pybind11.h>
#include "PyOpaqueTypes.hpp"
#include <pybind11/operators.h>
#include <list>
#include <dds/core/External.hpp>
#include <dds/core/Optional.hpp>
#include <rti/core/OptionalValue.hpp>
#include <dds/core/xtypes/DynamicType.hpp>

#ifdef _MSC_VER
    #undef PLATFORM
#endif

namespace py = pybind11;

PYBIND11_DECLARE_HOLDER_TYPE(T, dds::core::external<T>);

namespace pyrti {

using DefInitFunc = std::function<void()>;
using ClassInitFunc = std::function<DefInitFunc()>;
using ClassInitList = std::list<ClassInitFunc>;
using DefInitVector = std::vector<DefInitFunc>;

template<typename T>
void process_inits(py::module&, ClassInitList&);

template<typename T, typename... Bases>
void init_class_defs(py::class_<T, Bases...>& cls);

template<typename T, typename... Bases>
DefInitFunc init_class(py::object& parent, const std::string& cls_name)
{
    py::class_<T, Bases...> cls(parent, cls_name.c_str());

    return ([cls]() mutable { init_class_defs<T>(cls); });
}

template<typename from_class, typename to_class, typename py_to_class>
void add_conversion(
        py_to_class& cls,
        const std::string& doc = "Convert to less-specific type.")
{
    cls.def(py::init<const from_class&>(), py::arg("value"), doc.c_str());

    py::implicitly_convertible<from_class, to_class>();
}

template<typename from_class, typename to_class, typename py_to_class>
void add_conversion(
        py_to_class&& cls,
        const std::string& doc = "Convert to less-specific type.")
{
    cls.def(py::init<const from_class&>(), py::arg("value"), doc.c_str());

    py::implicitly_convertible<from_class, to_class>();
}


template<typename T>
bool has_value(const T& opt)
{
#if rti_connext_version_gte(6, 0, 1)
    return opt.has_value();
#else
    return opt.is_set();
#endif
}

template<template<class> class U, class T>
const T& get_value(const U<T>& opt)
{
#if rti_connext_version_gte(6, 0, 1)
    return opt.value();
#else
    return opt.get();
#endif
}

template<template<typename> class U, typename T>
T& get_value(U<T>& opt)
{
#if rti_connext_version_gte(6, 0, 1)
    return opt.value();
#else
    return opt.get();
#endif
}

#if rti_connext_version_lt(6, 1, 0)
static const int LISTENER_USE_COUNT_MIN = 2;

template<typename TEntity, typename TListenerPtr>
TListenerPtr get_listener(const TEntity& entity) {
    return entity.listener();
}

template<typename TEntity, typename TListenerPtr>
void set_listener(TEntity& entity, TListenerPtr listener) {
    if (nullptr == listener) entity.listener(listener, dds::core::status::StatusMask::none());
    else entity.listener(listener, dds::core::status::StatusMask::all());
}

template<typename TEntity, typename TListenerPtr>
void set_listener(
        TEntity& entity,
        TListenerPtr listener,
        const dds::core::status::StatusMask& mask) {
    entity.listener(listener, mask);
}

template<typename TPtr, typename TBasePtr>
TPtr downcast_listener_ptr(TBasePtr p) {
    return static_cast<TPtr>(p);
}
#else
static const int LISTENER_USE_COUNT_MIN = 1;

template<typename TEntity, typename TListenerPtr>
TListenerPtr get_listener(const TEntity& entity) {
    return entity.get_listener();
}

template<typename TEntity, typename TListenerPtr>
void set_listener(TEntity& entity, TListenerPtr listener) {
    if (nullptr == listener) entity.set_listener(listener, dds::core::status::StatusMask::none());
    else entity.set_listener(listener, dds::core::status::StatusMask::all());
}

template<typename TEntity, typename TListenerPtr>
void set_listener(
        TEntity& entity,
        TListenerPtr listener,
        const dds::core::status::StatusMask& mask) {
    entity.set_listener(listener, mask);
}

template<typename TPtr, typename TBasePtr>
TPtr downcast_listener_ptr(TBasePtr p) {
    return std::static_pointer_cast<typename TPtr::element_type>(p);
}
#endif

py::object py_cast_type(dds::core::xtypes::DynamicType&);

// Dummy classes
class PyPrimitiveType {
};
class PyBuiltinProfiles {
};

}  // namespace pyrti

namespace pybind11 { namespace detail {
template<typename T>
struct type_caster<dds::core::optional<T*>> {
    using base = type_caster_base<T>;

public:
    PYBIND11_TYPE_CASTER(
            dds::core::optional<T*>,
            _("Optional[") + base::name + _("]"));

    bool load(handle src, bool convert)
    {
        base base_caster;
        if (src.is_none()) {
            value.reset();
            return true;
        } else if (base_caster.load(src, convert)) {
            value = static_cast<T*>(base_caster);
            return true;
        }
        return false;
    }

    static handle cast(
            const dds::core::optional<T*>& src,
            return_value_policy r,
            handle h)
    {
        if (pyrti::has_value(src))
            return base::cast(pyrti::get_value(src), r, h);
        return py::none().release();
    }
};
}}  // namespace pybind11::detail

namespace pybind11 { namespace detail {
template<typename T>
struct type_caster<dds::core::optional<T>> {
    using base = type_caster<T>;

public:
    PYBIND11_TYPE_CASTER(
            dds::core::optional<T>,
            _("Optional[") + base::name + _("]"));

    bool load(handle src, bool convert)
    {
        base base_caster;
        if (src.is_none()) {
            value.reset();
            return true;
        } else if (base_caster.load(src, convert)) {
            value = static_cast<T&>(base_caster);
            return true;
        }
        return false;
    }

    static handle cast(
            const dds::core::optional<T>& src,
            return_value_policy r,
            handle h)
    {
        if (pyrti::has_value(src))
            return base::cast(pyrti::get_value(src), r, h);
        return py::none().release();
    }
};
}}  // namespace pybind11::detail

namespace pybind11 { namespace detail {
template<typename T>
struct type_caster<rti::core::optional_value<T>> {
    using base = type_caster<T>;

public:
    PYBIND11_TYPE_CASTER(
            rti::core::optional_value<T>,
            _("Optional[") + base::name + _("]"));

    bool load(handle src, bool convert)
    {
        base base_caster;
        if (src.is_none()) {
            value.reset();
            return true;
        } else if (base_caster.load(src, convert)) {
            value = rti::core::optional_value<T>(static_cast<T&>(base_caster));
            return true;
        }
        return false;
    }

    static handle cast(
            const rti::core::optional_value<T>& src,
            return_value_policy r,
            handle h)
    {
        if (pyrti::has_value(src))
            return base::cast(pyrti::get_value(src), r, h);
        return py::none().release();
    }
};
}}  // namespace pybind11::detail
