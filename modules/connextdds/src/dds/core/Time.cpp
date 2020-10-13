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

#include "PyConnext.hpp"
#include <pybind11/chrono.h>
#include <dds/core/Time.hpp>
#include <dds/core/Duration.hpp>
#include <rti/core/constants.hpp>

using namespace dds::core;
using timestamp = std::chrono::time_point<std::chrono::system_clock>;

namespace pyrti {

template<>
void init_class_defs(py::class_<Time>& cls)
{
    cls.def(py::init<>(),
            "Create a default Time object. The constructed Time object will "
            "represent 0 seconds and 0 nanoseconds.")
            .def(py::init<int32_t, uint32_t>(),
                 py::arg("sec"),
                 py::arg("nanosec") = 0,
                 "Create a Time object. The constructed Time object will "
                 "represent the given amount of time.")
            .def(py::init<Time&>(),
                 py::arg("other"),
                 "Copy-construct a Time object from another Time object.")
            .def(py::init([](timestamp ts) {
                     auto timestamp_nano = std::chrono::time_point_cast<
                             std::chrono::nanoseconds>(ts);
                     int64_t nanoseconds =
                             timestamp_nano.time_since_epoch().count();
                     int32_t sec = static_cast<int32_t>(
                             nanoseconds / rti::core::nanosec_per_sec);
                     uint32_t nanosec =
                             (nanoseconds % rti::core::nanosec_per_sec);
                     return Time(sec, nanosec);
                 }),
                 "Create a Time object from a timestamp.")
            .def_property(
                    "sec",
                    (int32_t(Time::*)() const) & Time::sec,
                    (void (Time::*)(int32_t)) & Time::sec,
                    "The number of seconds that are represented by this Time "
                    "object.")
            .def_property(
                    "nanosec",
                    (uint32_t(Time::*)() const) & Time::nanosec,
                    (void (Time::*)(uint32_t)) & Time::nanosec,
                    "The number of nanoseconds that are represented by this "
                    "Time object.")
            .def("compare",
                 &Time::compare,
                 py::arg("other"),
                 "Compare two Time objects.")
            .def(py::self > py::self,
                 "Check if this Time is greater than another.")
            .def(py::self >= py::self,
                 "Check if this Time is greater than or equal another.")
            .def(py::self == py::self,
                 "Check if this Time is equal to another.")
            .def(py::self != py::self,
                 "Check if this Time is not equal to another.")
            .def(py::self <= py::self,
                 "Check if this Time is less than or equal another.")
            .def(py::self < py::self,
                 "Check if this Time is less than another.")
            .def(py::self += Duration(), "Add a Duration to this Time object.")
            .def(py::self -= Duration(),
                 "Subtract a Duration from this Time object.")
            .def("to_millisecs",
                 &Time::to_millisecs,
                 "Convert this Time to milliseconds.")
            .def("to_microsecs",
                 &Time::to_microsecs,
                 "Convert this Time to microseconds.")
            .def("to_secs", &Time::to_secs, "Convert this Time to seconds.")
            .def_static(
                    "invalid",
                    &Time::invalid,
                    "Get a Time object representing an invalid amount of time.")
            .def_static(
                    "zero",
                    &Time::zero,
                    "Get a Time object representing zero time.")
            .def_static(
                    "maximum",
                    &Time::maximum,
                    "Get a Time object representing the maximum amount of "
                    "time.")
            .def_static(
                    "from_microsecs",
                    [](uint64_t microseconds) {
                        int32_t sec = static_cast<int32_t>(
                                microseconds / rti::core::microsec_per_sec);
                        uint32_t nanosec =
                                (microseconds % rti::core::microsec_per_sec)
                                * rti::core::nanosec_per_microsec;

                        return Time(sec, nanosec);
                    },
                    py::arg("microseconds"),
                    "Create a Time object from microseconds.")
            .def_static(
                    "from_millisecs",
                    [](uint64_t milliseconds) {
                        int32_t sec = static_cast<int32_t>(
                                milliseconds / rti::core::millisec_per_sec);
                        uint32_t nanosec =
                                (milliseconds % rti::core::millisec_per_sec)
                                * rti::core::nanosec_per_millisec;

                        return Time(sec, nanosec);
                    },
                    py::arg("milliseconds"),
                    "Create a Time object from millisecs.")
            .def_static(
                    "from_secs",
                    &Time::from_secs,
                    py::arg("seconds"),
                    "Create a Time object from seconds.")
            .def(py::self + Duration(), "Add a Time and Duration together.")
            .def(Duration() + py::self, "Add a Time and Duration together.")
            .def(py::self - Duration(), "Subtract a Duration from a Time.")
            .def(py::self - py::self,
                 "Calculate the duration between two times.");

    py::implicitly_convertible<timestamp, Time>();
}

template<>
void process_inits<Time>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable { return init_class<Time>(m, "Time"); });
}

}  // namespace pyrti
