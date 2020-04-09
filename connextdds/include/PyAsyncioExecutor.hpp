#pragma once

#include "PyConnext.hpp"
#include <functional>

namespace pyrti {

class PyAsyncioExecutor  {
public:
    template<typename T>
    static py::object run(std::function<T()> func) {
        auto instance = PyAsyncioExecutor::get_instance();
        py::object loop = instance.get_running_loop();
        py::object run_in_executor = loop.attr("run_in_executor");
        return run_in_executor(
            nullptr,
            std::function<T()>([func]() {
                py::gil_scoped_release release;
                return func();
            })
        );
    }

private:
    static std::unique_ptr<PyAsyncioExecutor> instance;
    py::object asyncio;
    py::object get_running_loop;

    PyAsyncioExecutor();
    static PyAsyncioExecutor& get_instance();
};

}
