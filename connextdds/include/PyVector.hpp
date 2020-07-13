#include "PyConnext.hpp"
#include <dds/core/vector.hpp>
#include <rti/core/Cookie.hpp>
#include <rti/core/TransportInfo.hpp>

namespace pyrti {

template<typename T>
static void init_dds_vector_defs(
        py::class_<dds::core::vector<T>>& cls,
        const std::string& name)
{
    cls.def(py::init<>(), ("Create an empty " + name).c_str())
            .def(py::init<size_t>(),
                 py::arg("size"),
                 ("Create a " + name + " with a preallocated size.").c_str())
            .def(py::init<const dds::core::vector<T>&>(),
                 py::arg("vector"),
                 ("Create a copy from another " + name + ".").c_str())
            .def(py::init([](py::iterable it) {
                dds::core::vector<T> v;
                v.reserve(py::len(it));
                int index = 0;
                for (py::handle h : it)
                    v.at(index++) = h.cast<T>();
                return v;
            }))
            .def(py::init<std::vector<T>>(),
                 py::arg("list"),
                 ("Create a " + name + " from a list of values.").c_str())
            .def("__len__",
                 &dds::core::vector<T>::size,
                 ("Get the length of the " + name + ".").c_str())
            .def(
                    "__getitem__",
                    [](dds::core::vector<T>& v, ssize_t i) { return v[i]; },
                    "Get the value at the specified index.")
            .def(
                    "__setitem__",
                    [](dds::core::vector<T>& v, ssize_t i, const T& b) {
                        v[i] = b;
                    },
                    "Set the value at the specified index.")
            .def(
                    "__iter__",
                    [](dds::core::vector<T>& v) {
                        return py::make_iterator(v.begin(), v.end());
                    },
                    py::keep_alive<0, 1>(),
                    "Iterate over the contents of the vector.")
            .def("clear",
                 &dds::core::vector<T>::clear,
                 ("Resize " + name + " to 0.").c_str())
            .def("resize",
                 (void (dds::core::vector<T>::*)(size_t))
                         & dds::core::vector<T>::resize,
                 py::arg("size"),
                 ("Resize " + name + ".").c_str())
            .def(py::self == py::self,
                 ("Compare " + name + "s for equality.").c_str())
            .def(py::self != py::self,
                 ("Compare " + name + "s for inequality.").c_str())
            .doc() =
            "A DDS standard container with functionality similar to a C++ "
            "vector.";

    py::implicitly_convertible<std::vector<T>, dds::core::vector<T>>();
    py::implicitly_convertible<py::iterable, dds::core::vector<T>>();
}

template<typename T>
static void init_dds_vector_buffer_defs(
        py::class_<dds::core::vector<T>>& cls,
        const std::string& name)
{
    cls.def(py::init([](py::buffer const b) {
                py::buffer_info info = b.request();
                if (info.format != py::format_descriptor<T>::format()
                    || info.ndim != 1) {
                    throw std::runtime_error("Incompatible buffer format!");
                }

                dds::core::vector<T> v(info.shape[0]);
                auto ptr = reinterpret_cast<T*>(info.ptr);
                std::copy(ptr, ptr + info.shape[0], v.begin());
                return v;
            }),
            py::arg("buffer"),
            ("Create a " + name + " from another Python buffer.").c_str())
            .def_buffer([](dds::core::vector<T>& v) {
                return py::buffer_info(
                        &*v.begin(),
                        { static_cast<ssize_t>(v.size()) },
                        { 1 });
            });

    py::implicitly_convertible<py::bytes, dds::core::vector<T>>();
    py::implicitly_convertible<py::buffer, dds::core::vector<T>>();
}

template<typename T>
static void init_dds_vector_nonbuffer_class(
        py::module& m,
        const std::string& cls_name,
        ClassInitList& l)
{
    l.push_back([m, cls_name]() mutable {
        py::class_<dds::core::vector<T>> cls(m, cls_name.c_str());
        return ([cls, cls_name]() mutable {
            init_dds_vector_defs<T>(cls, cls_name);
        });
    });
}

template<typename T>
static void init_dds_vector_buffer_class(
        py::module& m,
        const std::string& cls_name,
        ClassInitList& l)
{
    l.push_back([m, cls_name]() mutable {
        py::class_<dds::core::vector<T>> cls(
                m,
                cls_name.c_str(),
                py::buffer_protocol());
        return ([cls, cls_name]() mutable {
            init_dds_vector_buffer_defs<T>(cls, cls_name);
            init_dds_vector_defs<T>(cls, cls_name);
        });
    });
}

}  // namespace pyrti
