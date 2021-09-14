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
std::recursive_mutex PyAsyncioExecutor::lock;

PyAsyncioExecutor::PyAsyncioExecutor()
{
}

PyAsyncioExecutor& PyAsyncioExecutor::get_instance()
{
    std::lock_guard<std::recursive_mutex> lock(PyAsyncioExecutor::lock);
    if (!PyAsyncioExecutor::instance) {
        auto asyncio_module = py::module::import("asyncio");
        auto get_running_loop_func =  asyncio_module.attr("get_running_loop");
        auto atexit = py::module::import("atexit");
        PyAsyncioExecutor::instance.reset(new PyAsyncioExecutor());
        PyAsyncioExecutor::instance->asyncio = asyncio_module;
        PyAsyncioExecutor::instance->get_running_loop = get_running_loop_func;
        atexit.attr("register")(py::cpp_function([]() {
            auto ptr = PyAsyncioExecutor::instance.release();
            delete ptr;
        }));
    }
    return *PyAsyncioExecutor::instance;
}

}  // namespace pyrti