#include "PyConnext.hpp"
#include <pybind11/chrono.h>
#include <dds/core/Duration.hpp>

using namespace dds::core;

namespace pyrti {

template<>
void init_class_defs(py::class_<Duration>& cls) {
    cls
        .def(
            py::init<>(),
            "Create a Duration of 0 seconds."
        )
        .def(
            py::init<int32_t, uint32_t>(),
            py::arg("seconds"),
            py::arg("nanoseconds"),
            "Create a Duration of the specified seconds and nanoseconds."
        )
        .def(
            py::init<const std::chrono::duration<int64_t, std::nano>&>(),
            py::arg("duration"),
            "Create a Duration from a datetime.timedelta."
        )
        .def(
            py::init(
                [](double s) {
                    return Duration::from_secs(s);
                }
            ),
            py::arg("float_duration"),
            "Create a Duration from a floating point duration in seconds."
        )
        .def(
            py::init(
                [](int32_t s) {
                    return Duration(s, 0);
                }
            ),
            py::arg("seconds"),
            "Create a Duration from an integer number of seconds."
        )
        .def(
            py::init(
                [](const std::pair<int32_t, uint32_t>& d) {
                    return Duration(d.first, d.second);
                }
            ),
            py::arg("time_tuple"),
            "Create a Duration from a tuple of (sec, nanosec)."
        )
        .def_property(
            "sec",
            (int32_t (Duration::*)() const) &Duration::sec,
            (void (Duration::*)(int32_t)) &Duration::sec,
            "Get/set the number of seconds in the Duration."
        )
        .def_property(
            "nanosec",
            (uint32_t (Duration::*)() const) &Duration::nanosec,
            (void (Duration::*)(uint32_t)) &Duration::nanosec,
            "Get/set the number of nanoseconds in the Duration."
        )
        .def(
            "compare",
            &Duration::compare,
            py::arg("other"),
            "Compare this Duration to another. Returns -1 if this Duration is less, 0 if they are equal, and 1 if this Duration is greate"
        )
        .def(
            py::self > py::self,
            "Greater than comparison for Duration objects."
        )
        .def(
            py::self >= py::self,
            "Greater-than-or-equal comparison for Duration objects."
        )
        .def(
            py::self == py::self,
            "Compare Duration objects for equality."
        )
        .def(
            py::self != py::self,
            "Determine if Duration objects are unequal."
        )
        .def(
            py::self < py::self,
            "Less than comparison for Duration objects."
        )
        .def(
            py::self <= py::self,
            "Less-than-or-equal comparison for Duration object."
        )
        .def(
            py::self += py::self,
            "Add a Duration to another."
        )
        .def(
            py::self -= py::self,
            "Subtract a Duration from another."
        )
        .def(
            py::self + py::self,
            "Add two Duration objects."
        )
        .def(
            py::self - py::self,
            "Subtract two Duration objects."
        )
        .def(
            py::self * uint32_t(),
            "Multiply a Duration by an unsigned int."
        )
        .def(
            uint32_t() * py::self,
            "Multiply a Duration by an unsigned int."
        )
        .def(
            py::self / uint32_t(),
            "Divide a Duration by an unsigned int."
        )
        .def(
            "to_milliseconds",
            &Duration::to_millisecs,
            "Returns Duration in milliseconds."
        )
        .def(
            "to_microseconds",
            &Duration::to_microsecs,
            "Returns Duration in microseconds."
        )
        .def(
            "to_seconds",
            &Duration::to_secs,
            "Return the Duration in seconds."
        )
        .def(
            "to_timedelta",
            &Duration::to_chrono,
            "Return the datetime.timedelta conversion of this Duration."
        )
        .def(
            "__float__",
            [](const Duration& d) {
                return d.to_secs();
            },
            "Floating point value of Duration in seconds."
        )
        .def(
            "__int__",
            [](const Duration& d) {
                return d.to_microsecs();
            },
            "Integer value of Duration in microseconds."
        )
        .def_static(
            "zero",
            &Duration::zero,
            "Create a Duration of zero."
        )
        .def_static(
            "infinite",
            &Duration::infinite,
            "Create an infinite Duration"
        )
        .def_static(
            "automatic",
            &Duration::automatic,
            "Create a special Duration value to indicate a value should be selected automatically."
        )
        .def_static(
            "from_seconds",
            &Duration::from_secs,
            py::arg("seconds"),
            "Get a Duration from seconds"
        )
        .def_static(
            "from_milliseconds",
            &Duration::from_millisecs,
            py::arg("milliseconds"),
            "Get a Duration from milliseconds."
        )
        .def_static(
            "from_microseconds",
            &Duration::from_microsecs,
            py::arg("microseconds"),
            "Get a Duration from microseconds."
        );


        py::implicitly_convertible<double, Duration>();
        py::implicitly_convertible<std::chrono::duration<int64_t, std::nano>, Duration>();
        py::implicitly_convertible<const std::pair<int32_t, uint32_t>&, Duration>();
}

template<>
void process_inits<Duration>(py::module& m, ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return init_class<Duration>(m, "Duration");
        }
    );   
}

}
