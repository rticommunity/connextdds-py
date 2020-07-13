#include "PyConnext.hpp"
#include <rti/core/ContentFilterProperty.hpp>

using namespace rti::core;

namespace pyrti {

template<>
void init_class_defs(py::class_<ContentFilterProperty>& cls)
{
    cls.def_property_readonly(
               "content_filter_topic_name",
               [](const ContentFilterProperty& c) {
                   return c.content_filter_topic_name();
               },
               "Name of the ContentFilteredTopic associated with the "
               "DataReader.")
            .def_property_readonly(
                    "content_filter_topic_name",
                    &ContentFilterProperty::expression_parameters,
                    "The ContentFilteredTopic filter parameters.")
            .def_property_readonly(
                    "filter_class_name",
                    [](const ContentFilterProperty& c) {
                        return c.filter_class_name();
                    },
                    "Identifies the filter class this filter belongs to.")
            .def_property_readonly(
                    "filter_expression",
                    [](const ContentFilterProperty& c) {
                        return c.filter_expression();
                    },
                    "The filter expression.")
            .def_property_readonly(
                    "related_topic_name",
                    [](const ContentFilterProperty& c) {
                        return c.related_topic_name();
                    },
                    "The name of the ContentFilteredTopic's related Topic.")
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.");
}

template<>
void process_inits<ContentFilterProperty>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<ContentFilterProperty>(m, "ContentFilterProperty");
    });
}

}  // namespace pyrti
