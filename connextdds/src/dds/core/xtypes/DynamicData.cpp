#include "PyConnext.hpp"
#include <pybind11/numpy.h>
#include <dds/core/xtypes/DynamicData.hpp>
#include <dds/core/QosProvider.hpp>
#include "PyInitType.hpp"
#include "PyDynamicTypeMap.hpp"
#include "PyInitOpaqueTypeContainers.hpp"

using namespace dds::core::xtypes;
using namespace dds::topic;

INIT_OPAQUE_TYPE_CONTAINERS(dds::core::xtypes::DynamicData);

/* 
.def ( \
        "get_" type_str "_values", \
        [](const DynamicData& dd, const std::string& name, std::vector<type>& out) { \
            dd.get_values<type>(name, out); \
        }, \
        py::arg("name"), \
        py::arg("output"), \
        "Get multiple " type_info " values by field name."
        ) \
    .def ( \
        "get_" type_str "_values", \
        [](const DynamicData& dd, uint32_t index, std::vector<type>& out) { \
            dd.get_values<type>(index, out); \
        }, \
        py::arg("index"), \
        py::arg("output"), \
        "Get multiple " type_info " values by field name.") \
*/

namespace pyrti {
    template<>
    void init_dds_typed_topic_template(py::class_<pyrti::PyTopic<DynamicData>, pyrti::PyITopicDescription<DynamicData>, pyrti::PyIAnyTopic>& cls) {
        init_dds_typed_topic_base_template(cls); 
        cls
            .def(
                py::init(
                    [](
                        pyrti::PyDomainParticipant& dp,
                        const::std::string& name,
                        const dds::core::xtypes::DynamicType& type) {
                        pyrti::PyTopic<dds::core::xtypes::DynamicData> t(dp, name, type);
                        pyrti::PyDynamicTypeMap::add(t.type_name(), type);
                        return t;
                    }),
                py::arg("participant"),
                py::arg("topic_name"),
                py::arg("topic_type"),
                "Create a Topic with the given type."
            )
            .def(
                py::init(
                    [](pyrti::PyDomainParticipant& dp,
                        const::std::string& name,
                        const dds::core::xtypes::DynamicType& type,
                        const dds::topic::qos::TopicQos& qos,
                        pyrti::PyTopicListener<dds::core::xtypes::DynamicData>* listener,
                        const dds::core::status::StatusMask& mask) {
                        pyrti::PyTopic<dds::core::xtypes::DynamicData> t(
                                dp,
                                name,
                                type,
                                qos,
                                listener,
                                mask);
                        pyrti::PyDynamicTypeMap::add(t.type_name(), type);
                        return t;
                    }),
                py::arg("participant"),
                py::arg("topic_name"),
                py::arg("topic_type"),
                py::arg("qos"),
                py::arg("listener") = (pyrti::PyTopicListener<dds::core::xtypes::DynamicData>*) nullptr,
                py::arg_v("mask", dds::core::status::StatusMask::all(), "StatusMask.all"),
                py::keep_alive<1,6>(),
                "Create a Topic with the given type."
            );
    }

    template<>
    void init_dds_typed_datawriter_template(py::class_<pyrti::PyDataWriter<DynamicData>, pyrti::PyIEntity, pyrti::PyIAnyDataWriter>& cls) {
        init_dds_typed_datawriter_base_template(cls);
        cls
            .def(
                "create_data",
                [](pyrti::PyDataWriter<dds::core::xtypes::DynamicData>& dw) {
                    return dds::core::xtypes::DynamicData(pyrti::PyDynamicTypeMap::get(dw->type_name()));
                },
                "Create data of the writer's associated type and initialize it."
            )
            .def(
                "key_value",
                [](pyrti::PyDataWriter<dds::core::xtypes::DynamicData>& dw, const dds::core::InstanceHandle& handle) {
                    dds::core::xtypes::DynamicData d(pyrti::PyDynamicTypeMap::get(dw->type_name()));
                    dw.key_value(d, handle);
                    return d;
                },
                py::arg("handle"),
                "Retrieve the instance key that corresponds to an instance handle."
            );
            /*.def(
                "topic_instance_key_value",
                [](pyrti::PyDataWriter<dds::core::xtypes::DynamicData>& dw, const dds::core::InstanceHandle& handle) {
                    dds::core::xtypes::DynamicData d(pyrti::PyDynamicTypeMap::get(dw->type_name()));
                    dds::topic::TopicInstance<dds::core::xtypes::DynamicData> ti(handle, d);
                    dw.key_value(ti, handle);
                    return ti;
                },
                py::arg("handle"),
                "Retrieve the instance key that corresponds to an instance handle."
            );*/
    }

    template<>
    void init_dds_typed_datareader_template(py::class_<pyrti::PyDataReader<DynamicData>, pyrti::PyIDataReader>& cls) {
        init_dds_typed_datareader_base_template(cls);
        cls
            .def(
                "key_value",
                [](pyrti::PyDataReader<dds::core::xtypes::DynamicData>& dr, const dds::core::InstanceHandle& handle) {
                    dds::core::xtypes::DynamicType dt(pyrti::PyDynamicTypeMap::get(dr->type_name()));
                    dds::core::xtypes::DynamicData dd(dt);
                    dr.key_value(dd, handle);
                    return dd;
                },
                py::arg("handle"),
                "Retrieve the instance key that corresponds to an instance handle."
            )
            /*.def(
                "topic_instance_key_value",
                [](pyrti::PyDataReader<dds::core::xtypes::DynamicData>& dr, const dds::core::InstanceHandle& handle) {
                    dds::core::xtypes::DynamicType dt(pyrti::PyDynamicTypeMap::get(dr->type_name()));
                    dds::core::xtypes::DynamicData dd(dt);
                    dds::topic::TopicInstance<dds::core::xtypes::DynamicData> ti(handle, dd);
                    dr.key_value(ti, handle);
                    return ti;
                },
                py::arg("handle"),
                "Retrieve the instance key that corresponds to an instance handle."
            )*/
            .def(
                "read_next",
                [](pyrti::PyDataReader<dds::core::xtypes::DynamicData>& dr) -> py::object {
                    dds::core::xtypes::DynamicData data(pyrti::PyDynamicTypeMap::get(dr->type_name()));
                    dds::sub::SampleInfo info;
                    if (dr->read(data, info)) {
                        return py::cast(dds::sub::Sample<dds::core::xtypes::DynamicData>(data, info));
                    }
                    return py::cast(nullptr);
                },
                "Copy the next not-previously-accessed data value from the "
                "DataReader via a read operation."
            )
            .def(
                "take_next",
                [](pyrti::PyDataReader<dds::core::xtypes::DynamicData>& dr) -> py::object {
                    dds::core::xtypes::DynamicData data(pyrti::PyDynamicTypeMap::get(dr->type_name()));
                    dds::sub::SampleInfo info;
                    if (dr->take(data, info)) {
                        return py::cast(dds::sub::Sample<dds::core::xtypes::DynamicData>(data, info));
                    }
                    return py::cast(nullptr);
                },
                "Copy the next not-previously-accessed data value from the "
                "DataReader via a take operation."
            );
    }

    /*template<>
    void init_dds_typed_topic_instance_template(py::class_<dds::topic::TopicInstance<DynamicData>>& cls) {
        init_dds_typed_topic_instance_base_template(cls);
    }*/

    template<>
    void init_dds_typed_sample_template(py::class_<dds::sub::Sample<DynamicData>>& cls) {
        init_dds_typed_sample_base_template(cls);
    }

    template<typename T>
    void add_field_type(py::class_<DynamicData>& cls, const std::string& type_str, const std::string& type_info) {
        cls
            .def(
                ("get_" + type_str).c_str(),
                (T (DynamicData::*)(const std::string&) const) &DynamicData::value<T>,
                py::arg("name"),
                ("Get a " + type_info + " value by field name.").c_str()
            )
            .def(
                ("get_" + type_str).c_str(),
                [](const DynamicData &dd, uint32_t index) {
                    return dd.value<T>(index + 1);
                },
                py::arg("name"),
                ("Get a " + type_info + " value by field name.").c_str()
            )
            .def(
                ("set_" + type_str).c_str(),
                (void (DynamicData::*)(const std::string&, const T&)) &DynamicData::value<T>,
                py::arg("name"),
                py::arg("value"),
                ("Set a " + type_info + " value by name.").c_str()
            )
            .def(
                ("set_" + type_str).c_str(),
                [](DynamicData& dd, uint32_t index, const T& v) {
                    dd.value<T>(index + 1, v);
                },
                py::arg("index"),
                py::arg("value"),
                ("Set a " + type_info + " value by index.").c_str()
            );
    }

    template<typename T>
    void add_field_type_array(py::class_<DynamicData>& cls, const std::string& type_str, const std::string& type_info) {
        cls
            .def (
                ("get_" + type_str + "_values").c_str(),
                (std::vector<T> (DynamicData::*)(const std::string&) const) &DynamicData::get_values<T>,
                py::arg("name"),
                ("Get multiple " + type_info + " values by field name.").c_str()
            )
            .def (
                ("get_" + type_str + "_values").c_str(),
                [](const DynamicData& dd, uint32_t index) {
                    return dd.get_values<T>(index + 1);
                },
                py::arg("index"),
                ("Get multiple " + type_info + " values by field name.").c_str()
            )
            .def (
                ("set_" + type_str + "_values").c_str(),
                (void (DynamicData::*)(const std::string&, const std::vector<T>&)) &DynamicData::set_values<T>,
                py::arg("name"),
                py::arg("values"),
                ("Get multiple " + type_info + " values by field name.").c_str()
            )
            .def ( \
                ("set_" + type_str + "_values").c_str(),
                [](DynamicData& dd, uint32_t index, const std::vector<T>& v) {
                    dd.set_values<T>(index + 1, v);
                },
                py::arg("index"),
                py::arg("values"),
                ("Get multiple " + type_info + " values by field name.").c_str()
            );
    }
}

template<typename T>
static
py::object get_member(DynamicData& dd, TypeKind::inner_enum kind, T key) {
    switch (kind) {
        case TypeKind::BOOLEAN_TYPE:
            return py::cast(dd.value<bool>(key));
        case TypeKind::UINT_8_TYPE:
            return py::cast(dd.value<uint8_t>(key));
        case TypeKind::INT_16_TYPE:
            return py::cast(dd.value<int16_t>(key));
        case TypeKind::UINT_16_TYPE:
            return py::cast(dd.value<uint16_t>(key));
        case TypeKind::INT_32_TYPE:
            return py::cast(dd.value<int32_t>(key));
        case TypeKind::UINT_32_TYPE:
            return py::cast(dd.value<uint32_t>(key));
        case TypeKind::INT_64_TYPE:
            return py::cast(dd.value<rti::core::int64>(key));
        case TypeKind::UINT_64_TYPE:
            return py::cast(dd.value<rti::core::uint64>(key));
        case TypeKind::FLOAT_32_TYPE:
            return py::cast(dd.value<float>(key));
        case TypeKind::FLOAT_64_TYPE:
            return py::cast(dd.value<double>(key));
        case TypeKind::FLOAT_128_TYPE:
            return py::cast(dd.value<rti::core::LongDouble>(key));
        case TypeKind::CHAR_8_TYPE:
            return py::cast(dd.value<char>(key));
        case TypeKind::CHAR_16_TYPE:
            return py::cast(static_cast<wchar_t>(dd.value<DDS_Wchar>(key)));
        case TypeKind::STRING_TYPE:
            return py::cast(dd.value<std::string>(key));
        case TypeKind::WSTRING_TYPE:
            {
                std::vector<DDS_Wchar> v(dd.get_values<DDS_Wchar>(key));
                std::wstring s;
                for (auto c : v) {
                    s.push_back(static_cast<wchar_t>(c));
                }
                return py::cast(s);
            }
        default:
            return py::cast(dd.value<DynamicData>(key));
    }
}

template<typename T>
static
void set_member(DynamicData& dd, TypeKind::inner_enum kind, T key, py::object value) {
    switch (kind) {
        case TypeKind::BOOLEAN_TYPE:
            dd.value<bool>(key, py::cast<bool>(value));
            break;
        case TypeKind::UINT_8_TYPE:
            dd.value<uint8_t>(key, py::cast<uint8_t>(value));
            break;
        case TypeKind::INT_16_TYPE:
            dd.value<int16_t>(key, py::cast<int16_t>(value));
            break;
        case TypeKind::UINT_16_TYPE:
            dd.value<uint16_t>(key, py::cast<uint16_t>(value));
            break;
        case TypeKind::INT_32_TYPE:
            dd.value<int32_t>(key, py::cast<int32_t>(value));
            break;
        case TypeKind::UINT_32_TYPE:
            dd.value<uint32_t>(key, py::cast<uint32_t>(value));
            break;
        case TypeKind::INT_64_TYPE:
            dd.value<rti::core::int64>(key, py::cast<rti::core::int64>(value));
            break;
        case TypeKind::UINT_64_TYPE:
            dd.value<rti::core::uint64>(key, py::cast<rti::core::uint64>(value));
            break;
        case TypeKind::FLOAT_32_TYPE:
            dd.value<float>(key, py::cast<float>(value));
            break;
        case TypeKind::FLOAT_64_TYPE:
            dd.value<double>(key, py::cast<double>(value));
            break;
        case TypeKind::FLOAT_128_TYPE:
            dd.value<rti::core::LongDouble>(key, py::cast<rti::core::LongDouble>(value));
            break;
        case TypeKind::CHAR_8_TYPE:
            dd.value<char>(key, py::cast<char>(value));
            break;
        case TypeKind::CHAR_16_TYPE:
            dd.value<DDS_Wchar>(key, static_cast<DDS_Wchar>(py::cast<wchar_t>(value)));
            break;
        case TypeKind::STRING_TYPE:
            dd.value<std::string>(key, py::cast<std::string>(value));
            break;
        case TypeKind::WSTRING_TYPE:
            {
                std::wstring s = py::cast<std::wstring>(value);
                std::vector<DDS_Wchar> v;
                for (auto c : s) {
                    v.push_back(static_cast<DDS_Wchar>(c));
                }
                dd.set_values<DDS_Wchar>(key, v);
            }
            break;
        default:
            dd.value<DynamicData>(key, py::cast<DynamicData>(value));
    }
}


static
bool test_index(const DynamicData& dd, TypeKind::inner_enum kind, int index, const py::object& obj) {
    switch (kind) {
        case TypeKind::BOOLEAN_TYPE:
            return dd.value<bool>(index) == py::cast<bool>(obj);
        case TypeKind::UINT_8_TYPE:
            return dd.value<uint8_t>(index) == py::cast<uint8_t>(obj);
        case TypeKind::INT_16_TYPE:
            return dd.value<int16_t>(index) == py::cast<int16_t>(obj);
        case TypeKind::UINT_16_TYPE:
            return dd.value<uint16_t>(index) == py::cast<uint16_t>(obj);
        case TypeKind::INT_32_TYPE:
            return dd.value<int32_t>(index) == py::cast<int32_t>(obj);
        case TypeKind::UINT_32_TYPE:
            return dd.value<uint32_t>(index) == py::cast<uint32_t>(obj);
        case TypeKind::INT_64_TYPE:
            return dd.value<rti::core::int64>(index) == py::cast<rti::core::int64>(obj);
        case TypeKind::UINT_64_TYPE:
            return dd.value<rti::core::uint64>(index) == py::cast<rti::core::uint64>(obj);
        case TypeKind::FLOAT_32_TYPE:
            return dd.value<float>(index) == py::cast<float>(obj);
        case TypeKind::FLOAT_64_TYPE:
            return dd.value<double>(index) == py::cast<double>(obj);
        case TypeKind::FLOAT_128_TYPE:
            return dd.value<rti::core::LongDouble>(index) == py::cast<rti::core::LongDouble>(obj);
        case TypeKind::CHAR_8_TYPE:
            return dd.value<char>(index) == py::cast<char>(obj);
        case TypeKind::CHAR_16_TYPE:
            return dd.value<DDS_Wchar>(index) == static_cast<DDS_Wchar>(py::cast<wchar_t>(obj));
        case TypeKind::STRING_TYPE:
            return dd.value<std::string>(index) == py::cast<std::string>(obj);
        case TypeKind::WSTRING_TYPE:
            {
                std::wstring s1 = py::cast<std::wstring>(obj);
                std::vector<DDS_Wchar> v(dd.get_values<DDS_Wchar>(index));
                std::wstring s2;
                for (auto c : v) {
                    s2.push_back(static_cast<wchar_t>(c));
                }
                return s1 == s2;
            }
        default:
            return dd.value<DynamicData>(index) == py::cast<DynamicData>(obj);
    }
}

namespace pyrti {
    class PyDynamicDataFieldsIterator {
    public:
        PyDynamicDataFieldsIterator(DynamicData& dd, bool reversed) : _dd(dd) {
            if (reversed) {
                this->_index = dd.member_count();
                this->_end = 0;
                this->_step = -1;
            }
            else {
                this->_index = 1;
                this->_end = dd.member_count() + 1;
                this->_step = 1;
            }
        }

        std::string next() {
            if (this->_index == this->_end) throw py::stop_iteration();
            auto retval = this->_dd.member_info(this->_index).member_name();
            this->_index += this->_step;
            return retval;
        }

    private:
        DynamicData& _dd;
        int32_t _index;
        int32_t _step;
        int32_t _end;
    };


    class PyDynamicDataFieldsView {
    public:
        PyDynamicDataFieldsView(DynamicData& dd) : _dd(dd) {}
        
        bool contains(const std::string& field_name) {
            return _dd.member_exists_in_type(field_name);
        }

        PyDynamicDataFieldsIterator iter() {
            return PyDynamicDataFieldsIterator(this->_dd, false);
        }

        PyDynamicDataFieldsIterator reversed() {
            return PyDynamicDataFieldsIterator(this->_dd, true);
        }

        size_t len() {
            return this->_dd.member_count();
        }
        
    private:
        DynamicData& _dd;
    };


    class PyDynamicDataIndexIterator {
    public:
        PyDynamicDataIndexIterator(DynamicData& dd, bool reversed) : _dd(dd) {
            if (reversed) {
                this->_index = dd.member_count();
                this->_end = 0;
                this->_step = -1;
            }
            else {
                this->_index = 1;
                this->_end = dd.member_count() + 1;
                this->_step = 1;
            }
            this->_elem_kind = static_cast<const CollectionType&>(dd.type()).content_type().kind().underlying();
        }

        py::object next() {
            if (this->_index == this->_end) throw py::stop_iteration();
            auto retval = get_member(
                this->_dd,
                this->_elem_kind,
                this->_index);
            this->_index += this->_step;
            return retval;
        }

    private:
        DynamicData& _dd;
        TypeKind::inner_enum _elem_kind;
        int32_t _index;
        int32_t _step;
        int32_t _end;
    };


    class PyDynamicDataItemsIterator {
    public:
        PyDynamicDataItemsIterator(DynamicData& dd, bool reversed) : _dd(dd) {
            if (reversed) {
                this->_index = dd.member_count();
                this->_end = 0;
                this->_step = -1;
            }
            else {
                this->_index = 1;
                this->_end = dd.member_count() + 1;
                this->_step = 1;
            }
        }

        std::pair<std::string, py::object> next() {
            if (this->_index == this->_end) throw py::stop_iteration();
            auto field_name = this->_dd.member_info(this->_index).member_name();
            auto retval = std::pair<std::string, py::object>(
                field_name,
                get_member(this->_dd, this->_dd.member_info(field_name).member_kind().underlying(), field_name));
            this->_index += this->_step;
            return retval;
        }

    private:
        DynamicData& _dd;
        int32_t _index;
        int32_t _step;
        int32_t _end;
    };


    class PyDynamicDataItemsView {
    public:
        PyDynamicDataItemsView(DynamicData& dd) : _dd(dd) {}
        
        bool contains(const std::pair<std::string, py::object>& item) {
            return _dd.member_exists_in_type(item.first) &&
                test_index(this->_dd, this->_dd.member_info(item.first).member_kind().underlying(), this->_dd.member_index(item.first), item.second);
        }

        PyDynamicDataItemsIterator iter() {
            return PyDynamicDataItemsIterator(this->_dd, false);
        }

        PyDynamicDataItemsIterator reversed() {
            return PyDynamicDataItemsIterator(this->_dd, true);
        }

        size_t len() {
            return this->_dd.member_count();
        }
        
    private:
        DynamicData& _dd;
    };
}


template<>
void pyrti::init_class_defs(py::class_<DynamicData>& dd_class) {
    dd_class
        .def(
            py::init<
                const DynamicType&
            >()
        )
        .def(
            py::init<
                const DynamicType&,
                const rti::core::xtypes::DynamicDataProperty&>()
        );
    
    pyrti::add_field_type<char>(dd_class, "int8", "8-bit signed int");
    pyrti::add_field_type<uint8_t>(dd_class, "uint8", "8-bit unsigned int");
    pyrti::add_field_type<int16_t>(dd_class, "int16", "16-bit signed int");
    pyrti::add_field_type<uint16_t>(dd_class, "uint16", "16-bit unsigned int");
    pyrti::add_field_type<int32_t>(dd_class, "int32", "32-bit signed int");
    pyrti::add_field_type<uint32_t>(dd_class, "uint32", "32-bit unsigned int");
    pyrti::add_field_type<int>(dd_class, "int", "int (signed)");
    pyrti::add_field_type<unsigned int>(dd_class, "uint", "32-bit unsigned int");
    pyrti::add_field_type<rti::core::int64>(dd_class, "int64", "64-bit signed int");
    pyrti::add_field_type<rti::core::uint64>(dd_class, "uint64", "64-bit unsigned int");
    pyrti::add_field_type<long long>(dd_class, "longlong", "64-bit signed int");
    pyrti::add_field_type<unsigned long long>(dd_class, "ulonglong", "64-bit unsigned int");
    pyrti::add_field_type<float>(dd_class, "float32", "32-bit floating point");
    pyrti::add_field_type<double>(dd_class, "float64", "64-bit floating point");
    pyrti::add_field_type<rti::core::LongDouble>(dd_class, "float128", "128-bit floating point");
    pyrti::add_field_type<bool>(dd_class, "boolean", "boolean");
    pyrti::add_field_type<char>(dd_class, "char", "character");
    //DYNAMIC_DATA_GET_SET_DECL(dd_class, wchar_t, "wchar", "wide character");
    pyrti::add_field_type<std::string>(dd_class, "string", "string");
    //DYNAMIC_DATA_GET_SET_DECL(dd_class, std::wstring, "wstring", "wide string");
    pyrti::add_field_type<DynamicData>(dd_class, "complex", "complex data type");

    pyrti::add_field_type_array<char>(dd_class, "int8", "8-bit signed int");
    pyrti::add_field_type_array<uint8_t>(dd_class, "uint8", "8-bit unsigned int");
    pyrti::add_field_type_array<int16_t>(dd_class, "int16", "16-bit signed int");
    pyrti::add_field_type_array<uint16_t>(dd_class, "uint16", "16-bit unsigned int");
    pyrti::add_field_type_array<DDS_Long>(dd_class, "int32", "32-bit signed int");
    pyrti::add_field_type_array<DDS_UnsignedLong>(dd_class, "uint32", "32-bit unsigned int");
    pyrti::add_field_type_array<DDS_LongLong>(dd_class, "longlong", "64-bit signed int");
    pyrti::add_field_type_array<DDS_UnsignedLongLong>(dd_class, "ulonglong", "64-bit unsigned int");
    pyrti::add_field_type_array<float>(dd_class, "float32", "32-bit floating point");
    pyrti::add_field_type_array<double>(dd_class, "float64", "64-bit floating point");
    pyrti::add_field_type_array<char>(dd_class, "char", "character");

    dd_class
        .def(
            "get_wchar",
            [](const DynamicData& data, const std::string& name) {
                return static_cast<wchar_t>(data.value<uint16_t>(name));
            },
            py::arg("name"),
            "Get a wchar value by field name."
        )
        .def(
            "get_wchar",
            [](const DynamicData& data, uint32_t index) {
                return static_cast<wchar_t>(data.value<uint16_t>(index + 1));
            },
            py::arg("index"),
            "Get a wchar value by field index."
        )
        .def(
            "set_wchar",
            [](DynamicData& data, const std::string& name, wchar_t c) {
                DDS_Wchar value = static_cast<DDS_Wchar>(c);
                data.value<DDS_Wchar>(name, value);
            },
            py::arg("name"),
            py::arg("value"),
            "Get a wchar value by field name."
        )
        .def(
            "set_wchar",
            [](DynamicData& data, uint32_t index, wchar_t c) {
                DDS_Wchar value = static_cast<DDS_Wchar>(c);
                data.value<DDS_Wchar>(index + 1, value);
            },
            py::arg("index"),
            py::arg("value"),
            "Get a wchar value by field index."
        )
        .def(
            "get_wstring",
            [](const DynamicData& data, const std::string& name) {
                std::vector<DDS_Wchar> v(data.get_values<DDS_Wchar>(name));
                std::wstring s;
                for (auto c : v) {
                    s.push_back(static_cast<wchar_t>(c));
                }
                return s;
            },
            py::arg("name"),
            "Get a wstring value by field name."
        )
        .def(
            "get_wstring",
            [](const DynamicData& data, uint32_t index) {
                std::vector<DDS_Wchar> v(data.get_values<DDS_Wchar>(index + 1));
                std::wstring s;
                for (auto c : v) {
                    s.push_back(static_cast<wchar_t>(c));
                }
                return s;
            },
            py::arg("index"),
            "Get a wstring value by field index."
        )
        .def(
            "set_wstring",
            [](DynamicData& data, const std::string& name, const std::wstring& s) {
                std::vector<DDS_Wchar> v;
                for (auto c : s) {
                    v.push_back(static_cast<DDS_Wchar>(c));
                }
                data.set_values<DDS_Wchar>(name, v);
            },
            py::arg("name"),
            py::arg("value"),
            "Set a wstring by field name."
        )
        .def(
            "set_wstring",
            [](DynamicData& data, uint32_t index, const std::wstring& s) {
                std::vector<DDS_Wchar> v;
                for (auto c : s) {
                    v.push_back(static_cast<DDS_Wchar>(c));
                }
                data.set_values<DDS_Wchar>(index + 1, v);
            },
            py::arg("index"),
            py::arg("value"),
            "Get a wchar value by field index."
        )
        .def(
            "get_complex_values",
            [](const DynamicData& data, const std::string& name) {
                std::vector<DynamicData> v;
                auto info = data.member_info(name);
                if ((info.member_kind().underlying() & TypeKind::COLLECTION_TYPE) && 
                    !(info.element_kind().underlying() & TypeKind::PRIMITIVE_TYPE)) {
                    auto member = data.value<DynamicData>(name);
                    for (uint32_t i = 1; i <= info.element_count(); ++i) {
                        v.push_back(member.value<DynamicData>(i));
                    }
                    return v;
                }
                else {
                    throw py::key_error("member is not a collection of non-primitive values.");
                }
            },
            py::arg("name"),
            "Get a list of complex values by field name."
        )
        .def(
            "get_complex_values",
            [](const DynamicData& data, uint32_t index) {
                index += 1;
                std::vector<DynamicData> v;
                auto info = data.member_info(index);
                if ((info.member_kind().underlying() & TypeKind::COLLECTION_TYPE) && 
                    !(info.element_kind().underlying() & TypeKind::PRIMITIVE_TYPE))
                {
                    auto member = data.value<DynamicData>(index);
                    for (uint32_t i = 1; i <= info.element_count(); ++i) {
                        v.push_back(member.value<DynamicData>(i));
                    }
                    return v;
                }
                else {
                    throw py::key_error("member is not a collection of non-primitive values.");
                }
            },
            py::arg("index"),
            "Get a list of complex fields by index."
        )
            /* .def(
            "get_complex_values",
            [](DynamicData& data, const std::string& name, std::vector<DynamicData>& values) {
                auto info = data.member_info(name);
                if ((info.member_kind().underlying() & TypeKind::COLLECTION_TYPE) && !(info.element_kind().underlying() & TypeKind::PRIMITIVE_TYPE))
                {
                    values.clear();
                    auto member = data.value<DynamicData>(name);
                    for (int i = 1; i <= info.element_count(); ++i) {
                        values.push_back(member.value<DynamicData>(i));
                    }
                }
                else {
                    throw py::key_error("member is not a collection of non-primitive values.");
                }
            })
        .def(
            "get_complex_values",
            [](DynamicData& data, uint32_t index, std::vector<DynamicData>& values) {
                auto info = data.member_info(index);
                if ((info.member_kind().underlying() & TypeKind::COLLECTION_TYPE) && !(info.element_kind().underlying() & TypeKind::PRIMITIVE_TYPE))
                {
                    values.clear();
                    auto member = data.value<DynamicData>(index);
                    for (int i = 1; i <= info.element_count(); ++i) {
                        values.push_back(member.value<DynamicData>(i));
                    }
                }
                else {
                    throw py::key_error("member is not a collection of non-primitive values.");
                }
            }) */
        .def(
            "set_complex_values",
            [](DynamicData& data, const std::string& name, std::vector<DynamicData>& values) {
                auto info = data.member_info(name);
                if ((info.member_kind().underlying() & TypeKind::COLLECTION_TYPE) && 
                    !(info.element_kind().underlying() & TypeKind::PRIMITIVE_TYPE))
                {
                    auto member = data.value<DynamicData>(name);
                    for (uint32_t i = 0; i < values.size(); ++i) {
                        member.value<DynamicData>(i + 1, values[i]);
                    }
                }
                else {
                    throw py::key_error("member is not a collection of non-primitive values.");
                }
            },
            py::arg("name"),
            py::arg("values"),
            "Set a list of complex values by field name."
        )
        .def(
            "set_complex_values",
            [](DynamicData& data, uint32_t index, std::vector<DynamicData>& values) {
                index += 1;
                auto info = data.member_info(index);
                if ((info.member_kind().underlying() & TypeKind::COLLECTION_TYPE) && 
                    !(info.element_kind().underlying() & TypeKind::PRIMITIVE_TYPE))
                {
                    auto member = data.value<DynamicData>(index);
                    for (uint32_t i = 0; i < values.size(); ++i) {
                        member.value<DynamicData>(i + 1, values[i]);
                    }
                }
                else {
                    throw py::key_error("member is not a collection of non-primitive values.");
                }
            },
            py::arg("index"),
            py::arg("values"),
            "Set a list of complex values by index."
        )
        .def(
            "loan_value",
            (rti::core::xtypes::LoanedDynamicData (DynamicData::*)(const std::string&)) &DynamicData::loan_value,
            py::arg("name"),
            "Gets a view of a complex member."
        )
        .def(
            "loan_value",
            [](DynamicData& data, uint32_t index) {
                if (index == 0 || index > data.member_count()) throw py::index_error();
                return data.loan_value(index);
            },
            py::arg("index"),
            "Gets a view of a complex member."
        )
        .def(
            "loan_value",
            (rti::core::xtypes::LoanedDynamicData& (DynamicData::*)(rti::core::xtypes::LoanedDynamicData&, const std::string&)) &DynamicData::loan_value,
            py::arg("data"),
            py::arg("name"),
            "Gets a view of a complex member."
        )
        .def(
            "loan_value",
            [](DynamicData& data, rti::core::xtypes::LoanedDynamicData& loan, uint32_t index) -> rti::core::xtypes::LoanedDynamicData& {
                if (index == 0 || index > data.member_count()) throw py::index_error();
                return data.loan_value(loan, index);
            },
            py::arg("data"),
            py::arg("index"),
            "Gets a view of a complex member."
        )
        .def(
            "discriminator_value",
            &DynamicData::discriminator_value,
            "Obtains the value of the union discriminator (valid for UnionType only)."
        )
        .def(
            "clear_all_members",
            &DynamicData::clear_all_members,
            "Clear the contents of all data members of this object, including key members."
        )
        .def(
            "clear_optional_member",
            (void (DynamicData::*)(const std::string&)) &DynamicData::clear_optional_member,
            py::arg("name"),
            "Clear the contents of a single optional data member of this object."
        )
        .def(
            "clear_optional_member",
            [](DynamicData& data, uint32_t index) {
                if (index == 0 || index > data.member_count()) throw py::index_error();
                data.clear_optional_member(index);
            },
            py::arg("index"),
            "Clear the contents of a single optional data member of this object."
        )
        .def(
            "clear_member", 
            (void (DynamicData::*)(const std::string&)) &DynamicData::clear_member,
            py::arg("name"),
            "Clear the contents of a single data member of this object."
        )
        .def(
            "clear_member",
            [](DynamicData& data, uint32_t index) {
                if (index == 0 || index > data.member_count()) throw py::index_error();
                data.clear_member(index);
            },
            py::arg("index"),
            "Clear the contents of a single data member of this object."
        )
        .def(
            "set_buffer",
            &DynamicData::set_buffer,
            py::arg("storage"),
            py::arg("size"),
            "Associate a buffer with this dynamic data object.")
        .def_property_readonly(
            "estimated_max_buffer_size",
            &DynamicData::estimated_max_buffer_size,
            "Get the estimated maximum buffer size for a DynamicData object."
        )
        .def(
            "type",
            &DynamicData::type,
            "Gets this data type of this DynamicData."
        )
        .def(
            "type_kind",
            &DynamicData::type_kind,
            "Gets this data type kind of this DynamicData."
        )
        .def(
            "member_count",
            &DynamicData::member_count,
            "Get the number of members in this sample."
        )
        .def(
            "member_exists",
            (bool (DynamicData::*)(const std::string&) const) &DynamicData::member_exists,
            py::arg("name"),
            "Determine if an optional member is set by member name."
        )
        .def(
            "member_exists",
            [](const DynamicData& data, uint32_t index) {
                if (index == 0 || index > data.member_count()) throw py::index_error();
                return data.member_exists(index);
            },
            py::arg("index"),
            "Determine if an optional member is set by member index."
        )
        .def(
            "member_exists_in_type",
            (bool (DynamicData::*)(const std::string&) const) &DynamicData::member_exists_in_type,
            py::arg("name"),
            "Determine if a member with a particular name exists in the type."
        )
        .def(
            "member_exists_in_type",
            (bool (DynamicData::*)(uint32_t) const) &DynamicData::member_exists_in_type,
            py::arg("index"),
            "Determine if a member with a particular index exists in the type."
        )
        .def(
            "info",
            &DynamicData::info,
            "Returns info about this sample"
        )
        .def(
            "member_info",
            (rti::core::xtypes::DynamicDataMemberInfo (DynamicData::*)(const std::string&) const) &DynamicData::member_info,
            py::arg("name"),
            "Returns info about a member."
        )
        .def(
            "member_info",
            [](const DynamicData& data, uint32_t index) {
                if (index == 0 || index > data.member_count()) throw py::index_error();
                return data.member_info(index);
            },
            py::arg("index"),
            "Returns info about a member."
        )
        .def(
            "member_index",
            &DynamicData::member_index,
            py::arg("name"),
            "Translates from member name to member index."
        )
        .def(
            "is_member_key",
            (bool (DynamicData::*)(const std::string&) const) &DynamicData::is_member_key,
            py::arg("name"),
            "Returns whether a member is a key."
        )
        .def(
            "is_member_key",
            [](const DynamicData& data, uint32_t index) {
                if (index == 0 || index > data.member_count()) throw py::index_error();
                return data.is_member_key(index);
            },
            py::arg("index"),
            "Returns whether a member is a key."
        )
        .def(
            "__str__",
            [](const DynamicData& sample) {
                return rti::core::xtypes::to_string(sample);
            },
            "DynamicData value to string."
        )
        .def(
            "to_cdr_buffer",
            [](const dds::core::xtypes::DynamicData& sample) {
                std::vector<char> output;
                return rti::core::xtypes::to_cdr_buffer(output, sample);
            },
            "Serializes a DynamicData sample to CDR format"
        )
        .def(
            "from_cdr_buffer",
            [](dds::core::xtypes::DynamicData& sample, const std::vector<char>& buffer) {
                return rti::core::xtypes::from_cdr_buffer(sample, buffer);
            },
            py::arg("buffer"),
            "Populates a DynamicData sample by deserializing a CDR buffer."
        )
        .def(
            "fields",
            [](DynamicData& dd) {
                return pyrti::PyDynamicDataFieldsView(dd);
            }
        )
        .def(
            "items",
            [](DynamicData& dd) {
                return pyrti::PyDynamicDataItemsView(dd);
            }
        )
        .def(
            "__getitem__",
            [](DynamicData& dd, std::string& field_path) -> py::object {
                auto mi = dd.member_info(field_path);
                if (!dd.member_exists(mi.member_index())) {
                    return py::cast(nullptr);
                }

                return get_member(dd, mi.member_kind().underlying(), field_path);
            }
        )
        .def(
            "__setitem__",
            [](DynamicData& dd, std::string& field_path, py::object& value) {
                auto mi = dd.member_info(field_path);
                set_member(dd, mi.member_kind().underlying(), field_path, value);
            }
        )
        .def(
            "__getitem__",
            [](DynamicData& dd, size_t index) -> py::object {
                index += 1; // Python index starts at 0 by language convention
                auto type = dd.type();
                auto kind = type.kind().underlying();
                switch (kind) {
                    case TypeKind::STRUCTURE_TYPE:
                    case TypeKind::UNION_TYPE: {
                        auto mi = dd.member_info(index);
                        if (!dd.member_exists(index)) {
                            return py::cast(nullptr);
                        }
                        return get_member(dd, mi.member_kind().underlying(), index);
                    }
                    case TypeKind::ARRAY_TYPE:
                    case TypeKind::SEQUENCE_TYPE: {
                        auto collection_type = static_cast<const CollectionType&>(type);
                        return get_member(dd, collection_type.content_type().kind().underlying(), index);
                    }
                    default:
                        throw py::type_error("This DynamicData type does not support index access.");
                }
            }
        )
        .def(
            "__setitem__",
            [](DynamicData& dd, size_t index, py::object& value) {
                index += 1; // Python index starts at 0 by language convention
                auto type = dd.type();
                auto kind = type.kind().underlying();
                switch (kind) {
                    case TypeKind::STRUCTURE_TYPE:
                    case TypeKind::UNION_TYPE: {
                        auto mi = dd.member_info(index);
                        return set_member(dd, mi.member_kind().underlying(), index, value);
                    }
                    case TypeKind::ARRAY_TYPE:
                    case TypeKind::SEQUENCE_TYPE: {
                        auto collection_type = static_cast<const CollectionType&>(type);
                        return set_member(dd, collection_type.content_type().kind().underlying(), index, value);
                    }
                    default:
                        throw py::type_error("This DynamicData type does not support index access.");
                }
            }
        )
        .def(
            "__contains__",
            [](const DynamicData& dd, const std::string& field_name) {
                return dd.member_exists(field_name);
            }    
        )
        .def(
            "__contains__",
            [](const DynamicData& dd, const py::object& obj) {
                if (dd.type_kind().underlying() != TypeKind::ARRAY_TYPE &&
                    dd.type_kind().underlying() != TypeKind::SEQUENCE_TYPE) {
                    throw py::type_error("This DynamicData type does not support this operation");
                }
                auto kind = static_cast<const CollectionType&>(dd.type()).content_type().kind().underlying();
                for (uint32_t i = 1; i < dd.member_count(); ++i) {
                    if (test_index(dd, kind, i, obj)) return true;
                }
                return false;
            }    
        )
        .def(
            "__iter__",
            [](DynamicData& dd) {
                auto type = dd.type();
                auto kind = type.kind().underlying();
                switch (kind) {
                    case TypeKind::STRUCTURE_TYPE: {
                        return py::cast(pyrti::PyDynamicDataFieldsIterator(dd, false));
                    }
                    case TypeKind::ARRAY_TYPE:
                    case TypeKind::SEQUENCE_TYPE: {
                        return py::cast(pyrti::PyDynamicDataIndexIterator(dd, false));
                    }
                    default:
                        throw py::type_error("This DynamicData type does not support iteration");
                }
            },
            py::keep_alive<0, 1>()
        )
        .def(
            "__reverse__",
            [](DynamicData& dd) {
                auto type = dd.type();
                auto kind = type.kind().underlying();
                switch (kind) {
                    case TypeKind::STRUCTURE_TYPE: {
                        return py::cast(pyrti::PyDynamicDataFieldsIterator(dd, true));
                    }
                    case TypeKind::ARRAY_TYPE:
                    case TypeKind::SEQUENCE_TYPE: {
                        return py::cast(pyrti::PyDynamicDataIndexIterator(dd, true));
                    }
                    default:
                        throw py::type_error("This DynamicData type does not support iteration");
                }
            }
        );

    py::class_<topic_type_support<DynamicData>>(dd_class, "topic_type_support")
        .def_static(
            "register_type",
            &topic_type_support<DynamicData>::register_type,
            py::arg("participant"),
            py::arg("type_name"),
            "Register a type with a participant."
        )
        .def_static(
            "initialize_sample",
            &topic_type_support<DynamicData>::initialize_sample
        )
        .def_static(
            "to_cdr_buffer",
            &topic_type_support<DynamicData>::to_cdr_buffer,
            py::arg("buffer"),
            py::arg("sample"),
            py::arg("representation_id") = dds::core::policy::DataRepresentation::auto_id(),
            "Serialize sample to a CDR buffer."
        )
        .def_static(
            "from_cdr_buffer",
            &topic_type_support<DynamicData>::from_cdr_buffer,
            py::arg("sample"),
            py::arg("buffer"),
            "Deserialize a sample from a CDR buffer."
        );

    py::class_<pyrti::PyDynamicDataFieldsView>(dd_class, "FieldView")
        .def(
            py::init<
                DynamicData&
            >()
        )
        .def(
            "__iter__",
            &pyrti::PyDynamicDataFieldsView::iter,
            py::keep_alive<0, 1>()
        )
        .def(
            "__reverse__",
            &pyrti::PyDynamicDataFieldsView::reversed
        )
        .def(
            "__contains__",
            &pyrti::PyDynamicDataFieldsView::contains
        )
        .def(
            "__len__",
            &pyrti::PyDynamicDataFieldsView::len
        );

    py::class_<pyrti::PyDynamicDataFieldsIterator>(dd_class, "FieldIterator")
        .def(
            py::init<
                DynamicData&,
                bool
            >()      
        )
        .def(
            "__next__",
            &pyrti::PyDynamicDataFieldsIterator::next
        );

    py::class_<pyrti::PyDynamicDataIndexIterator>(dd_class, "IndexIterator")
        .def(
            py::init<
                DynamicData&,
                bool
            >()
        )
        .def(
            "__next__",
            &pyrti::PyDynamicDataIndexIterator::next
        );

    py::class_<pyrti::PyDynamicDataItemsView>(dd_class, "ItemView")
        .def(
            py::init<
                DynamicData&
            >()
        )
        .def(
            "__iter__",
            &pyrti::PyDynamicDataItemsView::iter,
            py::keep_alive<0, 1>()
        )
        .def(
            "__reverse__",
            &pyrti::PyDynamicDataItemsView::reversed
        )
        .def(
            "__contains__",
            &pyrti::PyDynamicDataItemsView::contains
        )
        .def(
            "__len__",
            &pyrti::PyDynamicDataItemsView::len
        );

    py::class_<pyrti::PyDynamicDataItemsIterator>(dd_class, "ItemsIterator")
        .def(
            py::init<
                DynamicData&,
                bool
            >()
        )
        .def(
            "__next__",
            &pyrti::PyDynamicDataItemsIterator::next
        );

    pyrti::init_type<dds::core::xtypes::DynamicData>(dd_class);
}

template<>
void pyrti::process_inits<DynamicData>(py::module& m, pyrti::ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return pyrti::init_class<DynamicData>(m, "DynamicData");
        }
    );
}