/*
 * (c) 2021 Copyright, Real-Time Innovations, Inc.  All rights reserved.
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

#if rti_connext_version_gte(6, 0, 0, 0)

namespace pyrti{

class PyThreadContext {
public:
    PyThreadContext();
    ~PyThreadContext();
    void unregister();
private:
    bool _unregistered;
};

}

#endif