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

uint8 = typing.NewType('uint8', int)
int8 = uint8  # workaround for CODEGENII-1753
char = typing.NewType('char', int)

int16 = typing.NewType('int16', int)
uint16 = typing.NewType('uint16', int)
wchar = typing.NewType('wchar', int)

int32 = typing.NewType('int32', int)
uint32 = typing.NewType('uint32', int)

int64 = int
uint64 = typing.NewType('uint64', int)

float32 = typing.NewType('float32', float)
float64 = float
