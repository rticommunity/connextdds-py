#include "PyConnext.hpp"
#include "PyNamespaces.hpp"


PYBIND11_MODULE(connextdds, m) {
    pyrti::ClassInitList cls_init_funcs;
    pyrti::DefInitVector def_init_funcs;
    init_misc(m, cls_init_funcs);
    init_namespace_dds(m, cls_init_funcs);
    init_namespace_rti(m, cls_init_funcs);

    bool stuck = false;
    while(!cls_init_funcs.empty()) {
        auto starting_size = cls_init_funcs.size();
        for (auto it = cls_init_funcs.begin(); it != cls_init_funcs.end();) {
            try {
                auto def_init = (*it)();
                def_init_funcs.push_back(def_init);
                it = cls_init_funcs.erase(it);
            }
            catch(...) {
                if (stuck) throw;
                ++it;
            }
        }
        if (cls_init_funcs.size() == starting_size) {
            /* We did not make any progress on class initialization
               which means we are stuck */
            stuck = true;
        }
    }
    for (auto func : def_init_funcs) {
        func();
    }
}