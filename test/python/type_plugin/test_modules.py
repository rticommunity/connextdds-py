#
# (c) 2022 Copyright, Real-Time Innovations, Inc.  All rights reserved.
#
# RTI grants Licensee a license to use, modify, compile, and create derivative
# works of the Software solely for use with RTI products.  The Software is
# provided "as is", with no warranty of any type, including any warranty for
# fitness for any purpose. RTI is under no obligation to maintain or support
# the Software.  RTI shall not be liable for any incidental or consequential
# damages arising out of the use or inability to use the software.
#

import rti.idl as idl

from common_types import Point, Point3D

def test_modules():
    com = idl.get_module('::com')
    com.foo = idl.get_module('::com::foo')
    com.bar = idl.get_module('::com::bar')

    assert com.foo is idl.get_module('::com::foo')
    assert com.bar is idl.get_module('::com::bar')

    com.foo.Point = Point
    com.bar.Point = Point3D

    assert com.foo.Point is Point
    assert com.bar.Point is Point3D

    # Getting the point again returns the same SimpleNamespace object
    com.foo = idl.get_module('::com::foo')
    assert com.foo.Point is Point
    assert com.bar.Point is Point3D

