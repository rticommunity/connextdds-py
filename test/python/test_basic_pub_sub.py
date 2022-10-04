 #
 # (c) 2020 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 #
 # RTI grants Licensee a license to use, modify, compile, and create derivative
 # works of the Software solely for use with RTI products.  The Software is
 # provided "as is", with no warranty of any type, including any warranty for
 # fitness for any purpose. RTI is under no obligation to maintain or support
 # the Software.  RTI shall not be liable for any incidental or consequential
 # damages arising out of the use or inability to use the software.
 #

from rti.types.builtin import String
from test_utils.fixtures import *

def test_basic_pubsub():
    fx = PubSubFixture(None, String)
    for i in range(5):
        fx.send_and_check(String(f"Hello World {i}"))