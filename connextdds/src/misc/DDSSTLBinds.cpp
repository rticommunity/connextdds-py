#include "PyConnext.hpp"
#include <pybind11/stl_bind.h>
#include "PyEntity.hpp"
#include "PyAnyTopic.hpp"
#include "PyAnyDataReader.hpp"
#include "PyAnyDataWriter.hpp"
#include "PyCondition.hpp"

void init_dds_stl_bindings(py::module& m) {
    py::bind_vector<std::vector<bool>>(m, "BoolSeq");
    py::bind_vector<std::vector<uint8_t>>(m, "OctetSeq", py::buffer_protocol());
    py::bind_vector<std::vector<char>>(m, "CharSeq", py::buffer_protocol());
    py::bind_vector<std::vector<uint16_t>>(m, "UnsignedShortSeq", py::buffer_protocol());
    py::bind_vector<std::vector<int16_t>>(m, "ShortSeq", py::buffer_protocol());
    py::bind_vector<std::vector<uint32_t>>(m, "UnsignedLongSeq", py::buffer_protocol());
    py::bind_vector<std::vector<int32_t>>(m, "LongSeq", py::buffer_protocol());
    py::bind_vector<std::vector<rti::core::uint64>>(m, "UnsignedLongLongSeq", py::buffer_protocol());
    py::bind_vector<std::vector<rti::core::int64>>(m, "LongLongSeq", py::buffer_protocol());
    py::bind_vector<std::vector<float>>(m, "FloatSeq", py::buffer_protocol());
    py::bind_vector<std::vector<double>>(m, "DoubleSeq", py::buffer_protocol());
    py::bind_vector<std::vector<rti::core::LongDouble>>(m, "LongDoubleSeq");
    py::bind_vector<std::vector<wchar_t>>(m, "WcharSeq");
    py::bind_vector<std::vector<std::string>>(m, "StringSeq");
    py::bind_vector<std::vector<std::wstring>>(m, "WstringSeq");
    py::bind_vector<std::vector<pyrti::PyICondition*>>(m, "IConditionSeq");
    py::bind_vector<std::vector<pyrti::PyCondition>>(m, "ConditionSeq");
    py::bind_vector<std::vector<dds::core::xtypes::Member>>(m, "MemberSeq");
    py::bind_vector<std::vector<dds::core::xtypes::EnumMember>>(m, "EnumMemberSeq");
    py::bind_vector<std::vector<dds::core::xtypes::UnionMember>>(m, "UnionMemberSeq");
    py::bind_vector<std::vector<dds::core::InstanceHandle>>(m, "InstanceHandleSeq");
    py::bind_vector<std::vector<pyrti::PyIEntity*>>(m, "IEntitySeq");
    py::bind_vector<std::vector<pyrti::PyEntity>>(m, "EntitySeq");
    py::bind_vector<std::vector<pyrti::PyDomainParticipant>>(m, "DomainParticipantSeq");
    py::bind_vector<std::vector<pyrti::PyAnyTopic>>(m, "AnyTopicSeq");
    py::bind_vector<std::vector<pyrti::PySubscriber>>(m, "SubscriberSeq");
    py::bind_vector<std::vector<pyrti::PyAnyDataReader>>(m, "AnyDataReaderSeq");
    py::bind_vector<std::vector<pyrti::PyPublisher>>(m, "PublisherSeq");
    py::bind_vector<std::vector<pyrti::PyAnyDataWriter>>(m, "AnyDataWriterSeq");
    py::bind_vector<std::vector<rti::core::EndpointGroup>>(m, "EndpointGroupSeq");
    py::bind_vector<std::vector<rti::core::Locator>>(m, "LocatorSeq");
    py::bind_vector<std::vector<rti::core::LocatorFilterElement>>(m, "FilterSeq");
    py::bind_vector<std::vector<rti::core::ChannelSettings>>(m, "ChannelSettingsSeq");
    py::bind_vector<std::vector<rti::core::TransportUnicastSettings>>(m, "TransportUnicastSettingsSeq");
    py::bind_vector<std::vector<std::pair<std::string, std::string>>>(m, "StringPairSeq");
    py::bind_map<std::map<std::string, std::string>>(m, "StringMap");

    py::implicitly_convertible<py::buffer, std::vector<uint8_t>>();
    py::implicitly_convertible<py::buffer, std::vector<char>>();
    py::implicitly_convertible<py::buffer, std::vector<uint16_t>>();
    py::implicitly_convertible<py::buffer, std::vector<int16_t>>();
    py::implicitly_convertible<py::buffer, std::vector<uint32_t>>();
    py::implicitly_convertible<py::buffer, std::vector<int32_t>>();
    py::implicitly_convertible<py::buffer, std::vector<rti::core::uint64>>();
    py::implicitly_convertible<py::buffer, std::vector<rti::core::int64>>();
    py::implicitly_convertible<py::buffer, std::vector<float>>();
    py::implicitly_convertible<py::buffer, std::vector<double>>();
    py::implicitly_convertible<py::iterable, std::vector<bool>>();
    py::implicitly_convertible<py::iterable, std::vector<uint8_t>>();
    py::implicitly_convertible<py::iterable, std::vector<char>>();
    py::implicitly_convertible<py::iterable, std::vector<uint16_t>>();
    py::implicitly_convertible<py::iterable, std::vector<int16_t>>();
    py::implicitly_convertible<py::iterable, std::vector<uint32_t>>();
    py::implicitly_convertible<py::iterable, std::vector<int32_t>>();
    py::implicitly_convertible<py::iterable, std::vector<rti::core::uint64>>();
    py::implicitly_convertible<py::iterable, std::vector<rti::core::int64>>();
    py::implicitly_convertible<py::iterable, std::vector<float>>();
    py::implicitly_convertible<py::iterable, std::vector<double>>();
    py::implicitly_convertible<py::iterable, std::vector<rti::core::LongDouble>>();
    py::implicitly_convertible<py::iterable, std::vector<wchar_t>>();
    py::implicitly_convertible<py::iterable, std::vector<std::string>>();
    py::implicitly_convertible<py::iterable, std::vector<std::wstring>>();
    
    py::implicitly_convertible<py::iterable, std::vector<pyrti::PyICondition*>>();
    py::implicitly_convertible<py::iterable, std::vector<pyrti::PyCondition>>();
    py::implicitly_convertible<py::iterable, std::vector<dds::core::xtypes::Member>>();
    py::implicitly_convertible<py::iterable, std::vector<dds::core::xtypes::EnumMember>>();
    py::implicitly_convertible<py::iterable, std::vector<dds::core::xtypes::UnionMember>>();
    py::implicitly_convertible<py::iterable, std::vector<dds::core::InstanceHandle>>();
    py::implicitly_convertible<py::iterable, std::vector<pyrti::PyIEntity*>>();
    py::implicitly_convertible<py::iterable, std::vector<pyrti::PyEntity>>();
    py::implicitly_convertible<py::iterable, std::vector<pyrti::PyDomainParticipant>>();
    py::implicitly_convertible<py::iterable, std::vector<pyrti::PyAnyTopic>>();
    py::implicitly_convertible<py::iterable, std::vector<pyrti::PySubscriber>>();
    py::implicitly_convertible<py::iterable, std::vector<pyrti::PyAnyDataReader>>();
    py::implicitly_convertible<py::iterable, std::vector<pyrti::PyPublisher>>();
    py::implicitly_convertible<py::iterable, std::vector<pyrti::PyAnyDataWriter>>();
    py::implicitly_convertible<py::iterable, std::vector<rti::core::EndpointGroup>>();
    py::implicitly_convertible<py::iterable, std::vector<rti::core::Locator>>();
    py::implicitly_convertible<py::iterable, std::vector<rti::core::LocatorFilterElement>>();
    py::implicitly_convertible<py::iterable, std::vector<rti::core::ChannelSettings>>();
    py::implicitly_convertible<py::iterable, std::vector<rti::core::TransportUnicastSettings>>();
    py::implicitly_convertible<py::iterable, std::vector<std::pair<std::string, std::string>>>();
}
