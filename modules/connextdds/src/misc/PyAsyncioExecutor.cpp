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

#include "PyAsyncioExecutor.hpp"

namespace pyrti {

std::unique_ptr<PyAsyncioExecutor> PyAsyncioExecutor::instance(nullptr);

PyAsyncioExecutor::PyAsyncioExecutor()
{
}

PyAsyncioExecutor& PyAsyncioExecutor::get_instance()
{
    if (!PyAsyncioExecutor::instance) {
        PyAsyncioExecutor::instance.reset(new PyAsyncioExecutor());
        PyAsyncioExecutor::instance->asyncio = py::module::import("asyncio");
        PyAsyncioExecutor::instance->get_running_loop =
                PyAsyncioExecutor::instance->asyncio.attr("get_running_loop");
        auto atexit = py::module::import("atexit");
        atexit.attr("register")(py::cpp_function([]() {
            auto ptr = PyAsyncioExecutor::instance.release();
            delete ptr;
        }));
    }
    return *PyAsyncioExecutor::instance;
}

}  // namespace pyrti