/*
 * (c) 2021 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 *
 * RTI grants Licensee a license to use, modify, compile, and create derivative
 * works of the Software solely for use with RTI products.  The Software is
 * provided "as is", with no warranty of any type, including any warranty for
 * fitness for any purpose. RTI is under no obligation to maintain or support
 * the Software.  RTI shall not be liable for any incidental or consequential
 * damages arising out of the use or inability to use the software.
 */


#include "PyThreadContext.hpp"

#if rti_connext_version_gte(6, 0, 0, 0)

#include <rti/core/thread.hpp>

namespace pyrti {

PyThreadContext::PyThreadContext() : _unregistered(false) {}

PyThreadContext::~PyThreadContext() {
    if (!this->_unregistered) {
        rti::core::unregister_thread();
    }
}

void PyThreadContext::unregister() {
    rti::core::unregister_thread();
    this->_unregistered = true;
}

template<>
void init_class_defs(py::class_<PyThreadContext>& cls)
{
    cls.def(
            py::init<>(),
            "Creates a thread context that will unregister the thread on exit.")
        .def("__enter__",
            [](PyThreadContext& tc) -> PyThreadContext& {
                return tc;
            })
        .def(
            "__exit__",
            [](PyThreadContext& tc, py::object, py::object, py::object) {
                tc.unregister(); 
            });
}

template<>
void process_inits<PyThreadContext>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<PyThreadContext>(m, "ThreadContext");
    });
}

}  // namespace pyrti

#endif