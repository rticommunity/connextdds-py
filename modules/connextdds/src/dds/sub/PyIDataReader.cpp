#include "PyConnext.hpp"
#include "PyAnyDataReader.hpp"
#include "PyEntity.hpp"
#include "PyDataReader.hpp"

namespace pyrti {

template<>
void init_class_defs(
        py::class_<PyIDataReader, PyIEntity, PyIAnyDataReader>& cls)
{
}

template<>
void process_inits<PyIDataReader>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<PyIDataReader, PyIEntity, PyIAnyDataReader>(
                m,
                "IDataReader");
    });
}

}  // namespace pyrti
