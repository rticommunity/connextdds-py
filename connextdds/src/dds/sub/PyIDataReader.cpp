#include "PyConnext.hpp"
#include "PyAnyDataReader.hpp"
#include "PyEntity.hpp"
#include "PyDataReader.hpp"

template<>
void pyrti::init_class_defs(py::class_<pyrti::PyIDataReader, pyrti::PyIEntity, pyrti::PyIAnyDataReader>& cls) {}

template<>
void pyrti::process_inits<pyrti::PyIDataReader>(py::module& m, pyrti::ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return pyrti::init_class<pyrti::PyIDataReader, pyrti::PyIEntity, pyrti::PyIAnyDataReader>(m, "IDataReader");
        }
    );
}
