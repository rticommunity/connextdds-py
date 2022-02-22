#
# (c) 2021 Copyright, Real-Time Innovations, Inc.  All rights reserved.
#
# RTI grants Licensee a license to use, modify, compile, and create derivative
# works of the Software solely for use with RTI products.  The Software is
# provided "as is", with no warranty of any type, including any warranty for
# fitness for any purpose. RTI is under no obligation to maintain or support
# the Software.  RTI shall not be liable for any incidental or consequential
# damages arising out of the use or inability to use the software.
#

import platform

if platform.system() == "Windows":
    import os
    pkg_dir = os.path.dirname(os.path.realpath(__file__))
    os.environ.setdefault("PATH","")
    os.environ["PATH"] = pkg_dir + os.pathsep + os.environ["PATH"]
