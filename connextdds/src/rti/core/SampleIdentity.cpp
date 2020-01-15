#include "PyConnext.hpp"
#include <rti/core/SampleIdentity.hpp>

using namespace rti::core;

template<>
void pyrti::init_class_defs(py::class_<SampleIdentity>& cls) {
     cls
        .def(
            py::init<>(),
            "Create a default SampleIdentity object."
        )
        .def(
            py::init<
                const Guid&,
                const SequenceNumber&
            >(),
            py::arg("writer_guid"),
            py::arg("sequence_number"),
            "Create a SampleIdentity with the provided Guid and "
            "SequenceNumber."
        )
        .def_property_readonly(
            "writer_guid",
            (Guid& (SampleIdentity::*)()) &SampleIdentity::writer_guid,
            "Get the 16-byte identifier identifying the virtual Guid."
        )
        .def_property_readonly(
            "sequence_number",
            (SequenceNumber& (SampleIdentity::*)()) &SampleIdentity::sequence_number,
            "Get the monotonically increasing 64-bit SequenceNumber that "
            "identifies the sample in the data source."
        )
        .def(
            "__str__",
            [](const SequenceNumber& sn) {
                std::ostringstream oss;
                oss << sn;
                return oss.str();
            }
        )
        .def_static(
            "automatic",
            &SampleIdentity::automatic,
            "Special value to indicate that RTI Connext will automatically "
            "assign the identity of a sample."
        )
        .def_static(
            "unknown",
            &SampleIdentity::unknown,
            "An invalid or unknown sample identity."
        )
        .def(
            py::self == py::self,
            "Test for equality."
        )
        .def(
            py::self != py::self,
            "Test for inequality."
        );
}

template<>
void pyrti::process_inits<SampleIdentity>(py::module& m, pyrti::ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return pyrti::init_class<SampleIdentity>(m, "SampleIdentity");
        }
    );
}
