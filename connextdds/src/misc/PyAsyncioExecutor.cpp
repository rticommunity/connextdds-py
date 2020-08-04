#include "PyAsyncioExecutor.hpp"

namespace pyrti {

std::unique_ptr<PyAsyncioExecutor> PyAsyncioExecutor::instance(nullptr);

PyAsyncioExecutor::PyAsyncioExecutor() {}

PyAsyncioExecutor &PyAsyncioExecutor::get_instance() {
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

} // namespace pyrti