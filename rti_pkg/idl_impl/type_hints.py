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

import typing

int8 = typing.NewType('int8', int)

int16 = typing.NewType('int16', int)
uint16 = typing.NewType('uint16', int)

int32 = typing.NewType('int32', int)
uint32 = typing.NewType('uint32', int)

int64 = int
uint64 = typing.NewType('uint64', int)

float32 = typing.NewType('float32', float)
float64 = float
