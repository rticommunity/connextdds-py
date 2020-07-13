#pragma once

#include "PyAnyDataReader.hpp"
#include "PyConnext.hpp"
#include <dds/core/cond/Condition.hpp>
#include <dds/core/cond/GuardCondition.hpp>
#include <dds/core/cond/StatusCondition.hpp>
#include <dds/sub/DataReader.hpp>
#include <dds/sub/cond/QueryCondition.hpp>
#include <dds/sub/cond/ReadCondition.hpp>

namespace pyrti {

class PyICondition {
public:
    virtual dds::core::cond::Condition get_condition() = 0;

    virtual void py_dispatch() = 0;

    virtual bool py_trigger_value() = 0;

    virtual ~PyICondition()
    {
    }
};

class PyIReadCondition : public PyICondition {
public:
    using PyICondition::PyICondition;

    virtual dds::sub::cond::ReadCondition get_read_condition() = 0;

    virtual const dds::sub::status::DataState py_state_filter() = 0;

    virtual const PyAnyDataReader py_data_reader() = 0;

    virtual void py_close() = 0;

    virtual bool py_closed() = 0;

    virtual ~PyIReadCondition()
    {
    }
};

class PyCondition : public dds::core::cond::Condition, public PyICondition {
public:
    using dds::core::cond::Condition::Condition;

    dds::core::cond::Condition get_condition() override
    {
        return dds::core::cond::Condition(*this);
    }

    void py_dispatch() override
    {
        this->dispatch();
    }

    bool py_trigger_value() override
    {
        return this->trigger_value();
    }

    virtual ~PyCondition()
    {
    }
};

class PyStatusCondition : public dds::core::cond::StatusCondition,
                          public PyICondition {
public:
    using dds::core::cond::StatusCondition::StatusCondition;

    dds::core::cond::Condition get_condition() override
    {
        return dds::core::cond::Condition(*this);
    }

    void py_dispatch() override
    {
        this->dispatch();
    }

    bool py_trigger_value() override
    {
        return this->trigger_value();
    }

    virtual ~PyStatusCondition()
    {
    }
};

class PyGuardCondition : public dds::core::cond::GuardCondition,
                         public PyICondition {
public:
    using dds::core::cond::GuardCondition::GuardCondition;

    PyGuardCondition() : dds::core::cond::GuardCondition()
    {
    }

    PyGuardCondition(const dds::core::cond::GuardCondition& gc)
            : dds::core::cond::GuardCondition(gc)
    {
    }

    dds::core::cond::Condition get_condition() override
    {
        return dds::core::cond::Condition(*this);
    }

    void py_dispatch() override
    {
        this->dispatch();
    }

    bool py_trigger_value() override
    {
        return this->trigger_value();
    }

    virtual ~PyGuardCondition()
    {
    }
};

class PyReadCondition : public dds::sub::cond::ReadCondition,
                        public PyIReadCondition {
public:
    using dds::sub::cond::ReadCondition::ReadCondition;

    PyReadCondition(const dds::sub::cond::ReadCondition& rc)
            : dds::sub::cond::ReadCondition(rc)
    {
    }

    dds::core::cond::Condition get_condition() override
    {
        return dds::core::cond::Condition(*this);
    }

    dds::sub::cond::ReadCondition get_read_condition() override
    {
        return *this;
    }

    void py_dispatch() override
    {
        this->dispatch();
    }

    bool py_trigger_value() override
    {
        return this->trigger_value();
    }

    const dds::sub::status::DataState py_state_filter() override
    {
        return this->state_filter();
    }

    const PyAnyDataReader py_data_reader() override
    {
        auto dr = this->data_reader();
        return PyAnyDataReader(dr);
    }

    void py_close() override
    {
        this->close();
    }

    bool py_closed() override
    {
        return this->closed();
    }

    virtual ~PyReadCondition()
    {
    }
};

class PyQueryCondition : public dds::sub::cond::QueryCondition,
                         public PyIReadCondition {
public:
    using dds::sub::cond::QueryCondition::QueryCondition;

    PyQueryCondition(const dds::sub::cond::QueryCondition& qc)
            : dds::sub::cond::QueryCondition(qc)
    {
    }

    dds::core::cond::Condition get_condition() override
    {
        return dds::core::cond::Condition(*this);
    }

    dds::sub::cond::ReadCondition get_read_condition() override
    {
        return dds::sub::cond::ReadCondition(*this);
    }

    void py_dispatch() override
    {
        this->dispatch();
    }

    bool py_trigger_value() override
    {
        return this->trigger_value();
    }

    const dds::sub::status::DataState py_state_filter() override
    {
        return this->state_filter();
    }

    const PyAnyDataReader py_data_reader() override
    {
        auto dr = this->data_reader();
        return PyAnyDataReader(dr);
    }

    void py_close() override
    {
        this->close();
    }

    bool py_closed() override
    {
        return this->closed();
    }

    virtual ~PyQueryCondition()
    {
    }
};

}  // namespace pyrti
