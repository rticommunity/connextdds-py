# This is an example toolchain file can be used to cross-compile the 
# RTI Connext Python API package
#
# Make sure to run configure.py, setting the toolchain argument to
# a properly modified toolchain and the Python root argument to point to 
# the cross-compiled Python build
#

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_VERSION 1)

# This example uses Buildroot and points to the
# output/host directory defined by an environment var
set(
    BUILDROOT_HOST_DIR
    $ENV{BUILDROOT_HOST_DIR}
)

set(
    CMAKE_PROGRAM_PATH
    ${BUILDROOT_HOST_DIR}/bin
)

# Modify compiler based on toolchain
set(CMAKE_C_COMPILER   ${BUILDROOT_HOST_DIR}/bin/arm-buildroot-linux-gnueabihf-gcc)
set(CMAKE_CXX_COMPILER ${BUILDROOT_HOST_DIR}/bin/arm-buildroot-linux-gnueabihf-g++)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# Since the toolchain file restricts CMake's find() functions,
# We must add the following Connext paths to CMake's search path
# to help FindRTIConnextDDS.cmake
list(
    APPEND CMAKE_FIND_ROOT_PATH
    ${BUILDROOT_HOST_DIR}/arm-buildroot-linux-gnueabihf/sysroot
    ${CONNEXTDDS_DIR}
    ${CONNEXTDDS_DIR}/bin
    ${CONNEXTDDS_DIR}/include/ndds
    ${Python_ROOT_DIR}
)

# The python module extension can be found by running the following in the target's
# shell:
#
# python -c "import distutils.sysconfig as s; print(s.get_config_var('EXT_SUFFIX'))"

set(PYTHON_MODULE_EXTENSION ".cpython-37m-arm-linux-gnueabihf.so")
