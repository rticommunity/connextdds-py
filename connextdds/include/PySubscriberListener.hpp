#pragma once

#include "PyConnext.hpp"
#include <dds/sub/SubscriberListener.hpp>
#include "PyAnyDataReaderListener.hpp"
#include "PyEntity.hpp"

namespace pyrti {
    class PySubscriberListener :
        public dds::sub::SubscriberListener,
        public pyrti::PyAnyDataReaderListener {
    public:
        using dds::sub::SubscriberListener::SubscriberListener;
        using pyrti::PyAnyDataReaderListener::PyAnyDataReaderListener;

        using dds::sub::SubscriberListener::on_data_on_readers;

        virtual
        void on_data_on_readers(
            pyrti::PySubscriber& sub
        ) = 0;

        virtual
        ~PySubscriberListener() {}
    };

    class PyNoOpSubscriberListener : 
        public pyrti::PySubscriberListener,
        public pyrti::PyNoOpAnyDataReaderListener {
    public:
        using pyrti::PySubscriberListener::PySubscriberListener;

        using pyrti::PyNoOpAnyDataReaderListener::on_requested_deadline_missed;
        using pyrti::PyNoOpAnyDataReaderListener::on_requested_incompatible_qos;
        using pyrti::PyNoOpAnyDataReaderListener::on_sample_rejected;
        using pyrti::PyNoOpAnyDataReaderListener::on_liveliness_changed;
        using pyrti::PyNoOpAnyDataReaderListener::on_data_available;
        using pyrti::PyNoOpAnyDataReaderListener::on_subscription_matched;
        using pyrti::PyNoOpAnyDataReaderListener::on_sample_lost;

        using pyrti::PySubscriberListener::on_data_on_readers;

        void on_data_on_readers(
            pyrti::PySubscriber& sub
        ) override {}

        virtual
        ~PyNoOpSubscriberListener() {}
    };

    template<class SLBase = pyrti::PySubscriberListener>
    class PySubscriberListenerTrampoline : public pyrti::PyAnyDataReaderListenerTrampoline<SLBase> {
    public:
        using pyrti::PyAnyDataReaderListenerTrampoline<SLBase>::PyAnyDataReaderListenerTrampoline;

        void on_data_on_readers(
            dds::sub::Subscriber& sub
        ) override {
            pyrti::PySubscriber s(sub);
            this->on_data_on_readers(s);
        }

        void on_data_on_readers(
            pyrti::PySubscriber& sub
        ) override {
            PYBIND11_OVERLOAD_PURE(
                void,
                SLBase,
                on_data_on_readers,
                sub
            );
        }

        virtual
        ~PySubscriberListenerTrampoline() {}
    };

    template <class SLBase = pyrti::PyNoOpSubscriberListener>
    class PyNoOpSubscriberListenerTrampoline : public pyrti::PyNoOpAnyDataReaderListenerTrampoline<SLBase> {
    public:
        using pyrti::PyNoOpAnyDataReaderListenerTrampoline<SLBase>::PyNoOpAnyDataReaderListenerTrampoline;
        
        void on_data_on_readers(
            dds::sub::Subscriber& sub
        ) override {
            pyrti::PySubscriber s(sub);
            this->on_data_on_readers(s);
        }

        void on_data_on_readers(
            pyrti::PySubscriber& sub
        ) override {
            PYBIND11_OVERLOAD(
                void,
                SLBase,
                on_data_on_readers,
                sub
            );
        }

        virtual
        ~PyNoOpSubscriberListenerTrampoline() {}
    };
}
