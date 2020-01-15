#pragma once

#include "PyConnext.hpp"
#include <dds/sub/DataReaderListener.hpp>
#include "PyDataReader.hpp"

namespace pyrti {
    template<typename T>
    class PyDataReader;

    template<typename T>
    class PyDataReaderListener : public dds::sub::DataReaderListener<T> {
    public:
        using dds::sub::DataReaderListener<T>::DataReaderListener;

        using dds::sub::DataReaderListener<T>::on_requested_deadline_missed;
        using dds::sub::DataReaderListener<T>::on_requested_incompatible_qos;
        using dds::sub::DataReaderListener<T>::on_sample_rejected;
        using dds::sub::DataReaderListener<T>::on_liveliness_changed;
        using dds::sub::DataReaderListener<T>::on_data_available;
        using dds::sub::DataReaderListener<T>::on_subscription_matched;
        using dds::sub::DataReaderListener<T>::on_sample_lost;

        virtual
        void on_requested_deadline_missed(
            pyrti::PyDataReader<T>& reader,
            const dds::core::status::RequestedDeadlineMissedStatus& status
        ) = 0;

        virtual
        void on_requested_incompatible_qos(
            pyrti::PyDataReader<T>& reader,
            const dds::core::status::RequestedIncompatibleQosStatus& status
        ) = 0;

        virtual
        void on_sample_rejected(
            pyrti::PyDataReader<T>& reader,
            const dds::core::status::SampleRejectedStatus& status
        ) = 0;

        virtual
        void on_liveliness_changed(
            pyrti::PyDataReader<T>& reader,
            const dds::core::status::LivelinessChangedStatus& status
        ) = 0;

        virtual
        void on_data_available(
            pyrti::PyDataReader<T>& reader
        ) = 0;

        virtual
        void on_subscription_matched(
            pyrti::PyDataReader<T>& reader,
            const dds::core::status::SubscriptionMatchedStatus& status
        ) = 0;

        virtual
        void on_sample_lost(
            pyrti::PyDataReader<T>& reader,
            const dds::core::status::SampleLostStatus& status
        ) = 0;
    };

    template<typename T>
    class PyNoOpDataReaderListener : public pyrti::PyDataReaderListener<T> {
    public:
        using pyrti::PyDataReaderListener<T>::PyDataReaderListener;

        using pyrti::PyDataReaderListener<T>::on_requested_deadline_missed;
        using pyrti::PyDataReaderListener<T>::on_requested_incompatible_qos;
        using pyrti::PyDataReaderListener<T>::on_sample_rejected;
        using pyrti::PyDataReaderListener<T>::on_liveliness_changed;
        using pyrti::PyDataReaderListener<T>::on_data_available;
        using pyrti::PyDataReaderListener<T>::on_subscription_matched;
        using pyrti::PyDataReaderListener<T>::on_sample_lost;

        void on_requested_deadline_missed(
            pyrti::PyDataReader<T>& reader,
            const dds::core::status::RequestedDeadlineMissedStatus& status
        ) override {}

        void on_requested_incompatible_qos(
            pyrti::PyDataReader<T>& reader,
            const dds::core::status::RequestedIncompatibleQosStatus& status
        ) override {}

        void on_sample_rejected(
            pyrti::PyDataReader<T>& reader,
            const dds::core::status::SampleRejectedStatus& status
        ) override {}

        void on_liveliness_changed(
            pyrti::PyDataReader<T>& reader,
            const dds::core::status::LivelinessChangedStatus& status
        ) override {}

        void on_data_available(
            pyrti::PyDataReader<T>& reader
        ) override {}

        void on_subscription_matched(
            pyrti::PyDataReader<T>& reader,
            const dds::core::status::SubscriptionMatchedStatus& status
        ) override {}

        void on_sample_lost(
            pyrti::PyDataReader<T>& reader,
            const dds::core::status::SampleLostStatus& status
        ) override {}
    };

    template<typename T, class DRLBase = pyrti::PyDataReaderListener<T>>
    class PyDataReaderListenerTrampoline : public DRLBase {
    public:
        using DRLBase::DRLBase;

        void on_requested_deadline_missed(
            dds::sub::DataReader<T>& reader,
            const dds::core::status::RequestedDeadlineMissedStatus& status
        ) override {
            pyrti::PyDataReader<T> dr(reader);
            this->on_requested_deadline_missed(dr, status);
        }
        
        void on_requested_incompatible_qos(
            dds::sub::DataReader<T>& reader,
            const dds::core::status::RequestedIncompatibleQosStatus& status
        ) override {
            pyrti::PyDataReader<T> dr(reader);
            this->on_requested_incompatible_qos(dr, status);
        }

        void on_sample_rejected(
            dds::sub::DataReader<T>& reader,
            const dds::core::status::SampleRejectedStatus& status
        ) override {
            pyrti::PyDataReader<T> dr(reader);
            this->on_sample_rejected(dr, status);
        }

        void on_liveliness_changed(
            dds::sub::DataReader<T>& reader,
            const dds::core::status::LivelinessChangedStatus& status
        ) override {
            pyrti::PyDataReader<T> dr(reader);
            this->on_liveliness_changed(dr, status);
        }

        void on_data_available(
            dds::sub::DataReader<T>& reader
        ) override {
            pyrti::PyDataReader<T> dr(reader);
            this->on_data_available(dr);
        }

        void on_subscription_matched(
            dds::sub::DataReader<T>& reader,
            const dds::core::status::SubscriptionMatchedStatus& status
        ) override {
            pyrti::PyDataReader<T> dr(reader);
            this->on_subscription_matched(dr, status);
        }

        void on_sample_lost(
            dds::sub::DataReader<T>& reader,
            const dds::core::status::SampleLostStatus& status
        ) override {
            pyrti::PyDataReader<T> dr(reader);
            this->on_sample_lost(dr, status);
        }

        void on_requested_deadline_missed(
            pyrti::PyDataReader<T>& reader,
            const dds::core::status::RequestedDeadlineMissedStatus& status
        ) override {
            PYBIND11_OVERLOAD_PURE(
                void,
                DRLBase,
                on_requested_deadline_missed,
                reader,
                status
            );
        }
        
        void on_requested_incompatible_qos(
            pyrti::PyDataReader<T>& reader,
            const dds::core::status::RequestedIncompatibleQosStatus& status
        ) override {
            PYBIND11_OVERLOAD_PURE(
                void,
                DRLBase,
                on_requested_incompatible_qos,
                reader,
                status
            );
        }

        void on_sample_rejected(
            pyrti::PyDataReader<T>& reader,
            const dds::core::status::SampleRejectedStatus& status
        ) override {
            PYBIND11_OVERLOAD_PURE(
                void,
                DRLBase,
                on_sample_rejected,
                reader,
                status
            );
        }

        void on_liveliness_changed(
            pyrti::PyDataReader<T>& reader,
            const dds::core::status::LivelinessChangedStatus& status
        ) override {
            PYBIND11_OVERLOAD_PURE(
                void,
                DRLBase,
                on_liveliness_changed,
                reader,
                status
            );
        }

        void on_data_available(
            pyrti::PyDataReader<T>& reader
        ) override {
            PYBIND11_OVERLOAD_PURE(
                void,
                DRLBase,
                on_data_available,
                reader
            );
        }

        void on_subscription_matched(
            pyrti::PyDataReader<T>& reader,
            const dds::core::status::SubscriptionMatchedStatus& status
        ) override {
            PYBIND11_OVERLOAD_PURE(
                void,
                DRLBase,
                on_subscription_matched,
                reader,
                status
            );
        }

        void on_sample_lost(
            pyrti::PyDataReader<T>& reader,
            const dds::core::status::SampleLostStatus& status
        ) override {
            PYBIND11_OVERLOAD_PURE(
                void,
                DRLBase,
                on_sample_lost,
                reader,
                status
            );
        }
    };

    template<typename T, class DRLBase = pyrti::PyNoOpDataReaderListener<T>>
    class PyNoOpDataReaderListenerTrampoline : public pyrti::PyDataReaderListenerTrampoline<T, DRLBase> {
    public:
        using pyrti::PyDataReaderListenerTrampoline<T, DRLBase>::PyDataReaderListenerTrampoline;

        using pyrti::PyDataReaderListenerTrampoline<T, DRLBase>::on_requested_deadline_missed;
        using pyrti::PyDataReaderListenerTrampoline<T, DRLBase>::on_requested_incompatible_qos;
        using pyrti::PyDataReaderListenerTrampoline<T, DRLBase>::on_sample_rejected;
        using pyrti::PyDataReaderListenerTrampoline<T, DRLBase>::on_liveliness_changed;
        using pyrti::PyDataReaderListenerTrampoline<T, DRLBase>::on_data_available;
        using pyrti::PyDataReaderListenerTrampoline<T, DRLBase>::on_subscription_matched;
        using pyrti::PyDataReaderListenerTrampoline<T, DRLBase>::on_sample_lost;

        void on_requested_deadline_missed(
            pyrti::PyDataReader<T>& reader,
            const dds::core::status::RequestedDeadlineMissedStatus& status
        ) override {
            PYBIND11_OVERLOAD(
                void,
                DRLBase,
                on_requested_deadline_missed,
                reader,
                status
            );
        }
        
        void on_requested_incompatible_qos(
            pyrti::PyDataReader<T>& reader,
            const dds::core::status::RequestedIncompatibleQosStatus& status
        ) override {
            PYBIND11_OVERLOAD(
                void,
                DRLBase,
                on_requested_incompatible_qos,
                reader,
                status
            );
        }

        void on_sample_rejected(
            pyrti::PyDataReader<T>& reader,
            const dds::core::status::SampleRejectedStatus& status
        ) override {
            PYBIND11_OVERLOAD(
                void,
                DRLBase,
                on_sample_rejected,
                reader,
                status
            );
        }

        void on_liveliness_changed(
            pyrti::PyDataReader<T>& reader,
            const dds::core::status::LivelinessChangedStatus& status
        ) override {
            PYBIND11_OVERLOAD(
                void,
                DRLBase,
                on_liveliness_changed,
                reader,
                status
            );
        }

        void on_data_available(
            pyrti::PyDataReader<T>& reader
        ) override {
            PYBIND11_OVERLOAD(
                void,
                DRLBase,
                on_data_available,
                reader
            );
        }

        void on_subscription_matched(
            pyrti::PyDataReader<T>& reader,
            const dds::core::status::SubscriptionMatchedStatus& status
        ) override {
            PYBIND11_OVERLOAD(
                void,
                DRLBase,
                on_subscription_matched,
                reader,
                status
            );
        }

        void on_sample_lost(
            pyrti::PyDataReader<T>& reader,
            const dds::core::status::SampleLostStatus& status
        ) override {
            PYBIND11_OVERLOAD(
                void,
                DRLBase,
                on_sample_lost,
                reader,
                status
            );
        }
    };

    template<typename T>
    void init_class_defs(py::class_<pyrti::PyDataReaderListener<T>, pyrti::PyDataReaderListenerTrampoline<T>>& cls) {
        cls
            .def(
                py::init<>()
            )
            .def(
                "on_requested_deadline_missed",
                (void (pyrti::PyDataReaderListener<T>::*)(pyrti::PyDataReader<T>&, const dds::core::status::RequestedDeadlineMissedStatus&)) &pyrti::PyDataReaderListener<T>::on_requested_deadline_missed,
                "Requested deadline missed callback."
            )
            .def(
                "on_requested_incompatible_qos",
                (void (pyrti::PyDataReaderListener<T>::*)(pyrti::PyDataReader<T>&, const dds::core::status::RequestedIncompatibleQosStatus&)) &pyrti::PyDataReaderListener<T>::on_requested_incompatible_qos,
                "Requested incompatible QoS callback."
            )
            .def(
                "on_sample_rejected",
                (void (pyrti::PyDataReaderListener<T>::*)(pyrti::PyDataReader<T>&, const dds::core::status::SampleRejectedStatus&)) &pyrti::PyDataReaderListener<T>::on_sample_rejected,
                "Sample rejected callback."
            )
            .def(
                "on_liveliness_changed",
                (void (pyrti::PyDataReaderListener<T>::*)(pyrti::PyDataReader<T>&, const dds::core::status::LivelinessChangedStatus&)) &pyrti::PyDataReaderListener<T>::on_liveliness_changed,
                "Liveliness changed callback."
            )
            .def(
                "on_data_available",
                (void (pyrti::PyDataReaderListener<T>::*)(pyrti::PyDataReader<T>&)) &pyrti::PyDataReaderListener<T>::on_data_available,
                "Data available callback."
            )
            .def(
                "on_subscription_matched",
                (void (pyrti::PyDataReaderListener<T>::*)(pyrti::PyDataReader<T>&, const dds::core::status::SubscriptionMatchedStatus&)) &pyrti::PyDataReaderListener<T>::on_subscription_matched,
                "Subscription matched callback.")
            .def(
                "on_sample_lost",
                (void (pyrti::PyDataReaderListener<T>::*)(pyrti::PyDataReader<T>&, const dds::core::status::SampleLostStatus&)) &pyrti::PyDataReaderListener<T>::on_sample_lost,
                "Sample lost callback."
            );
    }

    template<typename T>
    void init_class_defs(py::class_<pyrti::PyNoOpDataReaderListener<T>, pyrti::PyNoOpDataReaderListenerTrampoline<T>>& cls) {
        cls
            .def(
                py::init<>()
            )
            .def(
                "on_requested_deadline_missed",
                (void (pyrti::PyNoOpDataReaderListener<T>::*)(pyrti::PyDataReader<T>&, const dds::core::status::RequestedDeadlineMissedStatus&)) &pyrti::PyNoOpDataReaderListener<T>::on_requested_deadline_missed,
                "Requested deadline missed callback."
            )
            .def(
                "on_requested_incompatible_qos",
                (void (pyrti::PyNoOpDataReaderListener<T>::*)(pyrti::PyDataReader<T>&, const dds::core::status::RequestedIncompatibleQosStatus&)) &pyrti::PyNoOpDataReaderListener<T>::on_requested_incompatible_qos,
                "Requested incompatible QoS callback."
            )
            .def(
                "on_sample_rejected",
                (void (pyrti::PyNoOpDataReaderListener<T>::*)(pyrti::PyDataReader<T>&, const dds::core::status::SampleRejectedStatus&)) &pyrti::PyNoOpDataReaderListener<T>::on_sample_rejected,
                "Sample rejected callback."
            )
            .def(
                "on_liveliness_changed",
                (void (pyrti::PyNoOpDataReaderListener<T>::*)(pyrti::PyDataReader<T>&, const dds::core::status::LivelinessChangedStatus&)) &pyrti::PyNoOpDataReaderListener<T>::on_liveliness_changed,
                "Liveliness changed callback."
            )
            .def(
                "on_data_available",
                (void (pyrti::PyNoOpDataReaderListener<T>::*)(pyrti::PyDataReader<T>&)) &pyrti::PyNoOpDataReaderListener<T>::on_data_available,
                "Data available callback."
            )
            .def(
                "on_subscription_matched",
                (void (pyrti::PyNoOpDataReaderListener<T>::*)(pyrti::PyDataReader<T>&, const dds::core::status::SubscriptionMatchedStatus&)) &pyrti::PyNoOpDataReaderListener<T>::on_subscription_matched,
                "Subscription matched callback.")
            .def(
                "on_sample_lost",
                (void (pyrti::PyNoOpDataReaderListener<T>::*)(pyrti::PyDataReader<T>&, const dds::core::status::SampleLostStatus&)) &pyrti::PyNoOpDataReaderListener<T>::on_sample_lost,
                "Sample lost callback."
            );
    }

    template<typename T>
    void init_datareader_listener(py::object& o) {
        py::class_<pyrti::PyDataReaderListener<T>, pyrti::PyDataReaderListenerTrampoline<T>> drl(o, "DataReaderListener");
        py::class_<pyrti::PyNoOpDataReaderListener<T>, pyrti::PyNoOpDataReaderListenerTrampoline<T>> nodrl(o, "NoOpDataReaderListener");
        
        pyrti::init_class_defs(drl);
        pyrti::init_class_defs(nodrl);
    }
}

