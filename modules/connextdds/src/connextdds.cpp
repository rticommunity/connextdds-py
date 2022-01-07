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
#include "PyNamespaces.hpp"
#include "PyCoreUtils.hpp"


PYBIND11_MODULE(connextdds, m)
{
    pyrti::ClassInitList cls_init_funcs;
    pyrti::DefInitVector def_init_funcs;
    pyrti::DefInitVector late_init_funcs;
    init_misc_early(m, cls_init_funcs);
    init_namespace_dds(m, cls_init_funcs, late_init_funcs);
    init_namespace_rti(m, cls_init_funcs, late_init_funcs);

    bool stuck = false;
    while (!cls_init_funcs.empty()) {
        auto starting_size = cls_init_funcs.size();
        for (auto it = cls_init_funcs.begin(); it != cls_init_funcs.end();) {
            try {
                auto def_init = (*it)();
                def_init_funcs.push_back(def_init);
                it = cls_init_funcs.erase(it);
            } catch (...) {
                if (stuck)
                    throw;
                ++it;
            }
        }
        if (cls_init_funcs.size() == starting_size) {
            /* We did not make any progress on class initialization
               which means we are stuck */
            stuck = true;
        }
    }

    init_misc_late(m);

    for (auto func : def_init_funcs) {
        func();
    }

    for (auto func : late_init_funcs) {
        func();
    }

    auto core_utils_module = m.def_submodule(
            "core_utils",
            "Utilities from the RTI Connext DDS C implementation");
    init_core_utils(core_utils_module);
}
