#include "PyConnext.hpp"
#include <dds/core/Time.hpp>
#include <dds/core/Duration.hpp>

using namespace dds::core;

namespace pyrti {

template<>
void init_class_defs(py::class_<Time>& cls) {
    cls
        .def(
            py::init<>(),
            "Create a default Time object. The constructed Time object will represent 0 seconds and 0 nanoseconds."
        )
        .def(
            py::init<int32_t, uint32_t>(),
            py::arg("sec"),
            py::arg("nanosec") = 0,
            "Create a Time object. The constructed Time object will represent the given amount of time."
        )
        .def(
            py::init<Time&>(),
            py::arg("other"),
            "Copy-construct a Time object from another Time object."
        )
        .def_property(
            "sec", 
            (int32_t (Time::*)()const) &Time::sec,
            (void (Time::*)(int32_t)) &Time::sec,
            "The number of seconds that are represented by this Time object."
        )
        .def_property(
            "nanosec",
            (uint32_t (Time::*)()const) &Time::nanosec,
            (void (Time::*)(uint32_t)) &Time::nanosec,
            "The number of nanoseconds that are represented by this Time object."
        )
        .def(
            "compare",
            &Time::compare,
            py::arg("other"),
            "Compare two Time objects."
        )
        .def(
            py::self > py::self,
            /*"__gt__",
            [](const Time& t1, const Time& t2) {
                return t1 > t2;
            },*/
            "Check if this Time is greater than another."
        )
        .def(
            py::self >= py::self,
            /*"__ge__",
            [](const Time& t1, const Time& t2) {
                return t1 >= t2;
            },*/
            "Check if this Time is greater than or equal another."
        )
        .def(
            py::self == py::self,
            /*"__eq__",
            [](const Time& t1, const Time& t2) {
                return t1 == t2;
            },*/
            "Check if this Time is equal to another."
        )
        .def(
            py::self != py::self,
            /*"__ne__",
            [](const Time& t1, const Time& t2) {
                return t1 != t2;
            },*/
            "Check if this Time is not equal to another."
        )
        .def(
            py::self <= py::self,
            /*"__le__",
            [](const Time& t1, const Time& t2) {
                return t1 <= t2;
            },*/
            "Check if this Time is less than or equal another."
        )
        .def(
            py::self < py::self,
            /*"__lt__",
            [](const Time& t1, const Time& t2) {
                return t1 < t2;
            },*/
            "Check if this Time is less than another."
        )
        .def(
            py::self += Duration(),
            /*"__iadd__",
            [](Time& t, const Duration& d) {
                return t += d;
            },*/
            "Add a Duration to this Time object."
        )
        .def(
            py::self -= Duration(),
            /*"__isub__",
            [](Time& t, const Duration& d) {
                return t -= d;
            },*/
            "Subtract a Duration from this Time object."
        )
        .def(
            "to_millisecs",
            &Time::to_millisecs,
            "Convert this Time to milliseconds."
        )
        .def(
            "to_microsecs",
            &Time::to_microsecs,
            "Convert this Time to microseconds."
        )
        .def(
            "to_secs",
            &Time::to_secs,
            "Convert this Time to seconds."
        )
        .def_static(
            "invalid",
            &Time::invalid,
            "Get a Time object representing an invalid amount of time."
        )
        .def_static(
            "zero",
            &Time::zero,
            "Get a Time object representing zero time."
        )
        .def_static(
            "maximum",
            &Time::maximum,
            "Get a Time object representing the maximum amount of time."
        )
        .def_static(
            "from_microsecs",
            &Time::from_microsecs,
            py::arg("microseconds"),
            "Create a Time object from microseconds."
        )
        .def_static(
            "from_millisecs",
            &Time::from_millisecs,
            py::arg("milliseconds"),
            "Create a Time object from millisecs."
        )
        .def_static("from_secs", &Time::from_secs, "Create a Time object from seconds."
        )
        .def(
            py::self + Duration(),
            /*"__add__",
            [](const Time& t, const Duration& d) {
                return t + d;
            },*/
            "Add a Time and Duration together."
        )
        .def(
            Duration() + py::self,
            /*"__radd__",
            [](const Duration& d, const Time& t) {
                return d + t;
            },*/
            "Add a Time and Duration together."
        )
        .def(
            py::self - Duration(),
            /*"__sub__",
            [](const Time& t, const Duration& d) {
                return t - d;
            },*/
            "Subtract a Duration from a Time."
        )
        .def(
            py::self - py::self,
            "Calculate the duration between two times."
        );
}

template<>
void process_inits<Time>(py::module& m, ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return init_class<Time>(m, "Time");
        }
    );
}

}
