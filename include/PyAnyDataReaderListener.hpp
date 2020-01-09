#pragma once

#include "PyConnext.hpp"
#include <dds/sub/AnyDataReaderListener.hpp>
#include "PyAnyDataReader.hpp"

namespace pyrti {

    class PyAnyDataReaderListener : public dds::sub::AnyDataReaderListener {
    public:
        using dds::sub::AnyDataReaderListener::AnyDataReaderListener;

        using dds::sub::AnyDataReaderListener::on_requested_deadline_missed;
        using dds::sub::AnyDataReaderListener::on_requested_incompatible_qos;
        using dds::sub::AnyDataReaderListener::on_sample_rejected;
        using dds::sub::AnyDataReaderListener::on_liveliness_changed;
        using dds::sub::AnyDataReaderListener::on_data_available;
        using dds::sub::AnyDataReaderListener::on_subscription_matched;
        using dds::sub::AnyDataReaderListener::on_sample_lost;

        virtual
        void on_requested_deadline_missed(
            pyrti::PyAnyDataReader& reader,
            const dds::core::status::RequestedDeadlineMissedStatus& status
        ) = 0;

        virtual
        void on_requested_incompatible_qos(
            pyrti::PyAnyDataReader& reader,
            const dds::core::status::RequestedIncompatibleQosStatus& status
        ) = 0;

        virtual
        void on_sample_rejected(
            pyrti::PyAnyDataReader& reader,
            const dds::core::status::SampleRejectedStatus& status
        ) = 0;

        virtual
        void on_liveliness_changed(
            pyrti::PyAnyDataReader& reader,
            const dds::core::status::LivelinessChangedStatus& status
        ) = 0;

        virtual
        void on_data_available(
            pyrti::PyAnyDataReader& reader
        ) = 0;

        virtual
        void on_subscription_matched(
            pyrti::PyAnyDataReader& reader,
            const dds::core::status::SubscriptionMatchedStatus& status
        ) = 0;

        virtual
        void on_sample_lost(
            pyrti::PyAnyDataReader& reader,
            const dds::core::status::SampleLostStatus& status
        ) = 0;

        virtual
        ~PyAnyDataReaderListener() {}
    };




    class PyNoOpAnyDataReaderListener : public pyrti::PyAnyDataReaderListener {
    public:
        using pyrti::PyAnyDataReaderListener::PyAnyDataReaderListener;

        using pyrti::PyAnyDataReaderListener::on_requested_deadline_missed;
        using pyrti::PyAnyDataReaderListener::on_requested_incompatible_qos;
        using pyrti::PyAnyDataReaderListener::on_sample_rejected;
        using pyrti::PyAnyDataReaderListener::on_liveliness_changed;
        using pyrti::PyAnyDataReaderListener::on_data_available;
        using pyrti::PyAnyDataReaderListener::on_subscription_matched;
        using pyrti::PyAnyDataReaderListener::on_sample_lost;

        void on_requested_deadline_missed(
            pyrti::PyAnyDataReader& reader,
            const dds::core::status::RequestedDeadlineMissedStatus& status
        ) override {}

        void on_requested_incompatible_qos(
            pyrti::PyAnyDataReader& reader,
            const dds::core::status::RequestedIncompatibleQosStatus& status
        ) override {}

        void on_sample_rejected(
            pyrti::PyAnyDataReader& reader,
            const dds::core::status::SampleRejectedStatus& status
        ) override {}

        void on_liveliness_changed(
            pyrti::PyAnyDataReader& reader,
            const dds::core::status::LivelinessChangedStatus& status
        ) override{}

        void on_data_available(
            pyrti::PyAnyDataReader& reader
        ) override{}

        void on_subscription_matched(
            pyrti::PyAnyDataReader& reader,
            const dds::core::status::SubscriptionMatchedStatus& status
        ) override {}

        void on_sample_lost(
            pyrti::PyAnyDataReader& reader,
            const dds::core::status::SampleLostStatus& status
        ) override {}

        virtual
        ~PyNoOpAnyDataReaderListener() {}
    };



    template<class ADRLBase = pyrti::PyAnyDataReaderListener>
    class PyAnyDataReaderListenerTrampoline : public ADRLBase {
    public:
        using ADRLBase::ADRLBase;

        void on_requested_incompatible_qos(
            dds::sub::AnyDataReader& reader,
            const dds::core::status::RequestedIncompatibleQosStatus& status
        ) override {
            auto adr = pyrti::PyAnyDataReader(reader);
            this->on_requested_incompatible_qos(adr, status);
        }

        void on_sample_rejected(
            dds::sub::AnyDataReader& reader,
            const dds::core::status::SampleRejectedStatus& status
        ) override {
            auto adr = pyrti::PyAnyDataReader(reader);
            this->on_sample_rejected(adr, status);
        }

        void on_liveliness_changed(
            dds::sub::AnyDataReader& reader,
            const dds::core::status::LivelinessChangedStatus& status
        ) override {
            auto adr = pyrti::PyAnyDataReader(reader);
            this->on_liveliness_changed(adr, status);
        }

        void on_requested_deadline_missed(
            dds::sub::AnyDataReader& reader,
            const dds::core::status::RequestedDeadlineMissedStatus& status
        ) override {
            auto adr = pyrti::PyAnyDataReader(reader);
            this->on_requested_deadline_missed(adr, status);
        }

        void on_data_available(
            dds::sub::AnyDataReader& reader
        ) override {
            auto adr = pyrti::PyAnyDataReader(reader);
            this->on_data_available(adr);
        }

        void on_subscription_matched(
            dds::sub::AnyDataReader& reader,
            const dds::core::status::SubscriptionMatchedStatus& status
        ) override {
            auto adr = pyrti::PyAnyDataReader(reader);
            this->on_subscription_matched(adr, status);
        }

        void on_sample_lost(
            dds::sub::AnyDataReader& reader,
            const dds::core::status::SampleLostStatus& status
        ) override {
            auto adr = pyrti::PyAnyDataReader(reader);
            this->on_sample_lost(adr, status);
        }

        void on_requested_deadline_missed(
            pyrti::PyAnyDataReader& reader,
            const dds::core::status::RequestedDeadlineMissedStatus& status
        ) override {
            PYBIND11_OVERLOAD_PURE(
                void,
                ADRLBase,
                on_requested_deadline_missed,
                reader,
                status
            );
        }
        
        void on_requested_incompatible_qos(
            pyrti::PyAnyDataReader& reader,
            const dds::core::status::RequestedIncompatibleQosStatus& status
        ) override {
            PYBIND11_OVERLOAD_PURE(
                void,
                ADRLBase,
                on_requested_incompatible_qos,
                reader,
                status
            );
        }

        void on_sample_rejected(
            pyrti::PyAnyDataReader& reader,
            const dds::core::status::SampleRejectedStatus& status
        ) override {
            PYBIND11_OVERLOAD_PURE(
                void,
                ADRLBase,
                on_sample_rejected,
                reader,
                status
            );
        }

        void on_liveliness_changed(
            pyrti::PyAnyDataReader& reader,
            const dds::core::status::LivelinessChangedStatus& status
        ) override {
            PYBIND11_OVERLOAD_PURE(
                void,
                ADRLBase,
                on_liveliness_changed,
                reader,
                status
            );
        }

        void on_data_available(
            pyrti::PyAnyDataReader& reader
        ) override {
            PYBIND11_OVERLOAD_PURE(
                void,
                ADRLBase,
                on_data_available,
                reader
            );
        }

        void on_subscription_matched(
            pyrti::PyAnyDataReader& reader,
            const dds::core::status::SubscriptionMatchedStatus& status
        ) override {
            PYBIND11_OVERLOAD_PURE(
                void,
                ADRLBase,
                on_subscription_matched,
                reader,
                status
            );
        }

        void on_sample_lost(
            pyrti::PyAnyDataReader& reader,
            const dds::core::status::SampleLostStatus& status
        ) override {
            PYBIND11_OVERLOAD_PURE(
                void,
                ADRLBase,
                on_sample_lost,
                reader,
                status
            );
        }

        virtual
        ~PyAnyDataReaderListenerTrampoline() {}
    };



    template<class ADRLBase = pyrti::PyNoOpAnyDataReaderListener>
    class PyNoOpAnyDataReaderListenerTrampoline : public pyrti::PyAnyDataReaderListenerTrampoline<ADRLBase> {
    public:
        using pyrti::PyAnyDataReaderListenerTrampoline<ADRLBase>::PyAnyDataReaderListenerTrampoline;

        using pyrti::PyAnyDataReaderListenerTrampoline<ADRLBase>::on_requested_deadline_missed;
        using pyrti::PyAnyDataReaderListenerTrampoline<ADRLBase>::on_requested_incompatible_qos;
        using pyrti::PyAnyDataReaderListenerTrampoline<ADRLBase>::on_sample_rejected;
        using pyrti::PyAnyDataReaderListenerTrampoline<ADRLBase>::on_liveliness_changed;
        using pyrti::PyAnyDataReaderListenerTrampoline<ADRLBase>::on_data_available;
        using pyrti::PyAnyDataReaderListenerTrampoline<ADRLBase>::on_subscription_matched;
        using pyrti::PyAnyDataReaderListenerTrampoline<ADRLBase>::on_sample_lost;

        void on_requested_deadline_missed(
            pyrti::PyAnyDataReader& reader,
            const dds::core::status::RequestedDeadlineMissedStatus& status
        ) override {
            PYBIND11_OVERLOAD(
                void,
                ADRLBase,
                on_requested_deadline_missed,
                reader,
                status
            );
        }
        
        void on_requested_incompatible_qos(
            pyrti::PyAnyDataReader& reader,
            const dds::core::status::RequestedIncompatibleQosStatus& status
        ) override {
            PYBIND11_OVERLOAD(
                void,
                ADRLBase,
                on_requested_incompatible_qos,
                reader,
                status
            );
        }

        void on_sample_rejected(
            pyrti::PyAnyDataReader& reader,
            const dds::core::status::SampleRejectedStatus& status
        ) override {
            PYBIND11_OVERLOAD(
                void,
                ADRLBase,
                on_sample_rejected,
                reader,
                status
            );
        }

        void on_liveliness_changed(
            pyrti::PyAnyDataReader& reader,
            const dds::core::status::LivelinessChangedStatus& status
        ) override {
            PYBIND11_OVERLOAD(
                void,
                ADRLBase,
                on_liveliness_changed,
                reader,
                status
            );
        }

        void on_data_available(
            pyrti::PyAnyDataReader& reader
        ) override {
            PYBIND11_OVERLOAD(
                void,
                ADRLBase,
                on_data_available,
                reader
            );
        }

        void on_subscription_matched(
            pyrti::PyAnyDataReader& reader,
            const dds::core::status::SubscriptionMatchedStatus& status
        ) override {
            PYBIND11_OVERLOAD(
                void,
                ADRLBase,
                on_subscription_matched,
                reader,
                status
            );
        }

        void on_sample_lost(
            pyrti::PyAnyDataReader& reader,
            const dds::core::status::SampleLostStatus& status
        ) override {
            PYBIND11_OVERLOAD(
                void,
                ADRLBase,
                on_sample_lost,
                reader,
                status
            );
        }

        virtual
        ~PyNoOpAnyDataReaderListenerTrampoline() {}
    };
}
