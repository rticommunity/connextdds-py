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


#include "PyConnext.hpp"

#if rti_connext_version_gte(6, 1, 0)

#include <rti/core/PolicySettings.hpp>
#include "PySafeEnum.hpp"
#include "PyMaskType.hpp"

using namespace rti::core;

namespace pyrti {

template<>
void init_class_defs(py::class_<CompressionIdMask>& cls)
{
    cls.def_property_readonly_static(
                "ALL",
                [](py::object&) {
                    return rti::core::CompressionIdMask::all();
                },
                "All bits are set.")
            .def_property_readonly_static(
                "NONE",
                [](py::object&) {
                    return rti::core::CompressionIdMask::none();
                },
                "No Bits are set.")
            .def_property_readonly_static(
                "DEFAULT_PUBLICATION",
                [](py::object&) {
                    return rti::core::CompressionIdMask::default_publication();
                },
                "Default mask value for publication.")
            .def_property_readonly_static(
                "DEFAULT_SUBSCRIPTION",
                [](py::object&) {
                    return rti::core::CompressionIdMask::default_subscription();
                },
                "Default mask value for subscription.")
            .def_property_readonly_static(
                "ZLIB",
                [](py::object&) {
                    return rti::core::CompressionIdMask::zlib();
                },
                "Selects the built-in ZLIB compression algorithm.")
            .def_property_readonly_static(
                "BZIP2",
                [](py::object&) {
                    return rti::core::CompressionIdMask::bzip2();
                },
                "Selects the built-in BZIP2 compression algorithm.")
            .value(
                "LZ4",
                [](py::object&) {
                    return rti::core::CompressionIdMask::lz4();
                },
                "Selects the built-in LZ4 compression algorithm.");

}

template<>
void init_class_defs(py::class_<CompressionSettings>& cls)
{
    cls.def(py::init<>(),
            "Create a CompressionSettings object with default settings.")
            .def(
                    py::init<const CompressionIdMask>(),
                    py::arg("compression_ids"),
                    "Creates an instance with the given compression_ids.")
            .def(
                    py::init<const CompressionIdMask,
                        const uint32_t,
                        const int32_t>(),
                    py::arg("compression_ids"),
                    py::arg("writer_compression_level"),
                    py::arg("writer_compression_threshold"),
                    "Creates an instance with the given compression_ids, "
                    " writer_compression_level and writer_compression_threshold.")
            .def_property(
                    "compression_ids",
                    (CompressionIdMask(CompressionSettings::*)() const)
                            & CompressionSettings::compression_ids,
                    (CompressionSettings
                     & (CompressionSettings::*) (CompressionIdMask))
                            & ThreadSettings::compression_ids,
                    "Compression ID settings.")
            .def_property(
                    "writer_compression_level",
                    (uint32_t(CompressionSettings::*)() const)
                            & CompressionSettings::writer_compression_level,
                    (CompressionSettings & (CompressionSettings::*) (uint32_t))
                            & CompressionSettings::writer_compression_level,
                    "Writer compression level.")
            .def_property(
                    "writer_compression_threshold",
                    (int32_t(CompressionSettings::*)() const)
                            & CompressionSettings::writer_compression_threshold,
                    (CompressionSettings & (CompressionSettings::*) (int32_t))
                            & CompressionSettings::writer_compression_threshold,
                    "Writer compression threshold");
}

template<>
void process_inits<CompressionSettings>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        auto cls = init_mask_type<CompressionIdMask, uint64_t>(
                m,
                "CompressionIdMask",
                "Create a CompressionIdMask with no bits set.");
        return [cls]() mutable {
            init_class_defs<CompressionIdMask>(cls);
        };
    });

    l.push_back([m]() mutable {
        return init_class<ThreadSettings>(m, "ThreadSettings");
    });
}

}  // namespace pyrti

#endif