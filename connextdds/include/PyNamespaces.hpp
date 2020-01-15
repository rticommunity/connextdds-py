#pragma once

#include "PyConnext.hpp"

void init_namespace_dds_core_cond(py::module& m, pyrti::ClassInitList& l);
void init_namespace_dds_core_policy(py::module& m, pyrti::ClassInitList& l);
void init_namespace_dds_core_status(py::module& m, pyrti::ClassInitList& l);
void init_namespace_dds_core_xtypes(py::module& m, pyrti::ClassInitList& l);
void init_namespace_dds_core(py::module& m, pyrti::ClassInitList& l);
void init_namespace_dds_domain(py::module& m, pyrti::ClassInitList& l);
void init_namespace_dds_domain_qos(py::module& m, pyrti::ClassInitList& l);
void init_namespace_dds_pub_qos(py::module& m, pyrti::ClassInitList& l);
void init_namespace_dds_pub(py::module& m, pyrti::ClassInitList& l);
void init_namespace_dds_sub_cond(py::module& m, pyrti::ClassInitList& l);
void init_namespace_dds_sub_qos(py::module& m, pyrti::ClassInitList& l);
void init_namespace_dds_sub_status(py::module& m, pyrti::ClassInitList& l);
void init_namespace_dds_sub(py::module& m, pyrti::ClassInitList& l);
void init_namespace_dds_topic_qos(py::module& m, pyrti::ClassInitList& l);
void init_namespace_dds_topic(py::module& m, pyrti::ClassInitList& l);
void init_namespace_dds(py::module& m, pyrti::ClassInitList& l);
void init_namespace_rti_config(py::module& m, pyrti::ClassInitList& l);
void init_namespace_rti_core_cond(py::module& m, pyrti::ClassInitList& l);
void init_namespace_rti_core_policy(py::module& m, pyrti::ClassInitList& l);
void init_namespace_rti_core_status(py::module& m, pyrti::ClassInitList& l);
void init_namespace_rti_core_xtypes(py::module& m, pyrti::ClassInitList& l);
void init_namespace_rti_core(py::module& m, pyrti::ClassInitList& l);
void init_namespace_rti_domain(py::module& m, pyrti::ClassInitList& l);
void init_namespace_rti_pub(py::module& m, pyrti::ClassInitList& l);
void init_namespace_rti_sub_status(py::module& m, pyrti::ClassInitList& l);
void init_namespace_rti_sub(py::module& m, pyrti::ClassInitList& l);
void init_namespace_rti_topic(py::module& m, pyrti::ClassInitList& l);
void init_namespace_rti(py::module& m, pyrti::ClassInitList& l);
void init_misc(py::module& m, pyrti::ClassInitList& l);
