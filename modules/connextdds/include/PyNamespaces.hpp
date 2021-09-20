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

void init_namespace_dds_core_cond(py::module&, pyrti::ClassInitList&, pyrti::DefInitVector&);
void init_namespace_dds_core_policy(py::module&, pyrti::ClassInitList&, pyrti::DefInitVector&);
void init_namespace_dds_core_status(py::module&, pyrti::ClassInitList&, pyrti::DefInitVector&);
void init_namespace_dds_core_xtypes(py::module&, pyrti::ClassInitList&, pyrti::DefInitVector&);
void init_namespace_dds_core(py::module&, pyrti::ClassInitList&, pyrti::DefInitVector&);
void init_namespace_dds_domain(py::module&, pyrti::ClassInitList&, pyrti::DefInitVector&);
void init_namespace_dds_domain_qos(py::module&, pyrti::ClassInitList&, pyrti::DefInitVector&);
void init_namespace_dds_pub_qos(py::module&, pyrti::ClassInitList&, pyrti::DefInitVector&);
void init_namespace_dds_pub(py::module&, pyrti::ClassInitList&, pyrti::DefInitVector&);
void init_namespace_dds_sub_cond(py::module&, pyrti::ClassInitList&, pyrti::DefInitVector&);
void init_namespace_dds_sub_qos(py::module&, pyrti::ClassInitList&, pyrti::DefInitVector&);
void init_namespace_dds_sub_status(py::module&, pyrti::ClassInitList&, pyrti::DefInitVector&);
void init_namespace_dds_sub(py::module&, pyrti::ClassInitList&, pyrti::DefInitVector&);
void init_namespace_dds_topic_qos(py::module&, pyrti::ClassInitList&, pyrti::DefInitVector&);
void init_namespace_dds_topic(py::module&, pyrti::ClassInitList&, pyrti::DefInitVector&);
void init_namespace_dds(py::module&, pyrti::ClassInitList&, pyrti::DefInitVector&);
void init_namespace_rti_config(py::module&, pyrti::ClassInitList&, pyrti::DefInitVector&);
void init_namespace_rti_core_cond(py::module&, pyrti::ClassInitList&, pyrti::DefInitVector&);
void init_namespace_rti_core_policy(py::module&, pyrti::ClassInitList&, pyrti::DefInitVector&);
void init_namespace_rti_core_status(py::module&, pyrti::ClassInitList&, pyrti::DefInitVector&);
void init_namespace_rti_core_xtypes(py::module&, pyrti::ClassInitList&, pyrti::DefInitVector&);
void init_namespace_rti_core(py::module&, pyrti::ClassInitList&, pyrti::DefInitVector&);
void init_namespace_rti_domain(py::module&, pyrti::ClassInitList&, pyrti::DefInitVector&);
void init_namespace_rti_pub(py::module&, pyrti::ClassInitList&, pyrti::DefInitVector&);
void init_namespace_rti_sub_status(py::module&, pyrti::ClassInitList&, pyrti::DefInitVector&);
void init_namespace_rti_sub(py::module&, pyrti::ClassInitList&, pyrti::DefInitVector&);
void init_namespace_rti_topic(py::module&, pyrti::ClassInitList&, pyrti::DefInitVector&);
void init_namespace_rti_util(py::module&, pyrti::ClassInitList&, pyrti::DefInitVector&);
void init_namespace_rti(py::module&, pyrti::ClassInitList&, pyrti::DefInitVector&);
void init_heap_monitoring(py::module&, pyrti::ClassInitList&, pyrti::DefInitVector&);
#if rti_connext_version_gte(6, 1, 0, 0)
void init_network_capture(py::module&, pyrti::ClassInitList&, pyrti::DefInitVector&);
void init_activity_context(py::module&, pyrti::ClassInitList&, pyrti::DefInitVector&);
#endif
void init_misc_early(py::module&, pyrti::ClassInitList&);
void init_misc_late(py::module&);
