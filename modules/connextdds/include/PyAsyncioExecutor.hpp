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

#include "PyConnext.hpp"
#include <functional>

namespace pyrti {

class PYRTI_SYMBOL_HIDDEN PyAsyncioExecutor {
public:
    template<typename T>
    static py::object run(std::function<T()> func)
    {
        auto instance = PyAsyncioExecutor::get_instance();
        py::object loop = instance.get_running_loop();
        py::object run_in_executor = loop.attr("run_in_executor");
        return run_in_executor(nullptr, std::function<T()>([func]() -> T {
                                   py::gil_scoped_release release;
                                   return func();
                               }));
    }

private:
    static std::unique_ptr<PyAsyncioExecutor> instance;
    py::object asyncio;
    py::object get_running_loop;

    PyAsyncioExecutor();
    static PyAsyncioExecutor& get_instance();
};

}  // namespace pyrti
