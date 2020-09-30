#pragma once

#include <pybind11/pybind11.h>
#include "PyOpaqueTypes.hpp"
#include <pybind11/operators.h>

namespace py = pybind11;

namespace pyrti {

template<typename T>
py::class_<T> init_mask_type_no_int_constructor(
        py::object& m,
        const std::string& name,
        const std::string& noarg_constructor_doc_str =
                "Creates a mask with no bits set.")
{
    py::class_<T> mt(m, name.c_str());
    mt.def(py::init<>(), noarg_constructor_doc_str.c_str())
            .def(
                    "__getitem__",
                    [](const T& mask, std::size_t index) -> bool {
                        return mask[index];
                    },
                    "Get individual mask bit.")
            .def(
                    "__setitem__",
                    [](T& mask, std::size_t index, bool value) {
                        mask[index] = value;
                    },
                    "Set individual mask bit")
            .def("__contains__",
                 [](T& mask, T& other) { return (mask & other) == other; })
            .def("test",
                 &T::test,
                 py::arg("pos"),
                 "Test whether the mask bit at postition \"pos\" is set.")
            .def("test_all", &T::all, "Test if all bits are set.")
            .def("test_any", &T::any, "Test if any bits are set.")
            .def("test_none", &T::none, "Test if any bits are set.")
            .def("count",
                 &T::count,
                 "Returns the number of bits set in the mask.")
            .def("size",
                 &T::size,
                 "Returns the number of bits in the mask type.")
            .def(
                    "set",
                    [](T& mask) {
                        mask.set();
                        return mask;
                    },
                    "Set all bits in the mask.")
            .def(
                    "set",
                    [](T& mask, std::size_t pos, bool value) {
                        mask.set(pos, value);
                        return mask;
                    },
                    py::arg("pos"),
                    py::arg("value") = true,
                    "Set the mask bit at the specified position to the "
                    "provided value "
                    "(default: true).")
            .def(
                    "reset",
                    [](T& mask) {
                        mask.reset();
                        return mask;
                    },
                    "Clear all bits in the mask.")
            .def(
                    "reset",
                    [](T& mask, std::size_t pos) {
                        mask.reset(pos);
                        return mask;
                    },
                    py::arg("pos"),
                    "Clear the mask bit at the specified position.")
            .def(
                    "flip",
                    [](T& mask) {
                        mask.flip();
                        return mask;
                    },
                    "Flip all bits in the mask.")
            .def(
                    "flip",
                    [](T& mask, std::size_t pos) {
                        mask.flip(pos);
                        return mask;
                    },
                    py::arg("pos"),
                    "Flip the mask bit at the specified position.")
            .def(py::self == py::self, "Compare masks for equality.")
            .def(py::self != py::self, "Compare masks for inequality.")
            .def(
                    "__iand__",
                    [](T& mask, const T& other) {
                        mask &= other;
                        return mask;
                    },
                    py::is_operator(),
                    "Set mask to logical AND with another mask.")
            .def(
                    "__ior__",
                    [](T& mask, const T& other) {
                        mask |= other;
                        return mask;
                    },
                    py::is_operator(),
                    "Set mask to logical OR with another mask.")
            .def(
                    "__ixor__",
                    [](T& mask, const T& other) {
                        mask ^= other;
                        return mask;
                    },
                    py::is_operator(),
                    "Set mask to logical XOR with another mask.")
            .def(
                    "__ilshift__",
                    [](T& mask, std::size_t value) {
                        mask <<= value;
                        return mask;
                    },
                    py::is_operator(),
                    "Left shift bits in mask.")
            .def(
                    "__irshift__",
                    [](T& mask, std::size_t value) {
                        mask <<= value;
                        return mask;
                    },
                    py::is_operator(),
                    "Right shift bits in mask.")
            .def(
                    "__and__",
                    [](const T& mask, const T& other) {
                        return T(mask & other);
                    },
                    py::is_operator(),
                    "Bitwise logical AND of masks.")
            .def(
                    "__or__",
                    [](const T& mask, const T& other) {
                        return T(mask | other);
                    },
                    py::is_operator(),
                    "Bitwise logical OR of masks.")
            .def(
                    "__xor__",
                    [](const T& mask, const T& other) {
                        return T(mask ^ other);
                    },
                    py::is_operator(),
                    "Bitwise logical XOR of masks.")
            .def(
                    "__lshift__",
                    [](const T& mask, std::size_t value) {
                        return T(mask << value);
                    },
                    py::is_operator(),
                    "Left shift bits in mask.")
            .def(
                    "__rshift__",
                    [](const T& mask, std::size_t value) {
                        return T(mask >> value);
                    },
                    py::is_operator(),
                    "Right shift bits in mask.")
            .def(
                    "__str__",
                    [](T& mask) { return mask.to_string(); },
                    "Convert mask to string.")
            .def("__int__", &T::to_ullong, "Convert mask to int.");

    return mt;
}

template<typename T, typename INIT_TYPE>
py::class_<T> init_mask_type(
        py::object& m,
        const std::string& name,
        const std::string& noarg_constructor_doc_str =
                "Creates a mask with no bits set.")
{
    auto mt = init_mask_type_no_int_constructor<T>(
            m,
            name,
            noarg_constructor_doc_str);
    mt.def(py::init<INIT_TYPE>(),
           py::arg("value"),
           "Creates a mask from the bits in an integer.");

    py::implicitly_convertible<INIT_TYPE, T>();

    return mt;
}

}  // namespace pyrti
