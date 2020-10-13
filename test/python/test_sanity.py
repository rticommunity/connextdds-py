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

"""
Test Sanity
This test is to ensure that the testing framework itself is working
"""


class TestSanity:
    def test_one_plus_one(self):
        """
        This test ensures 1+1=2
        """
        assert 1 + 1 == 2
        print("If this is failing, we have some big problems")
