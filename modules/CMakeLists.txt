cmake_minimum_required(VERSION 3.15)
project(connext-py)
include(CheckCXXCompilerFlag)

file(TO_CMAKE_PATH ${CONNEXTDDS_DIR} CONNEXTDDS_DIR)
if (Python_ROOT_DIR)
    file(TO_CMAKE_PATH ${Python_ROOT_DIR} Python_ROOT_DIR)
endif()

list(APPEND CMAKE_FIND_ROOT_PATH
    ${CONNEXTDDS_DIR}
    ${CONNEXTDDS_DIR}/bin
    ${CONNEXTDDS_DIR}/lib/$ENV{CONNEXTDDS_ARCH}
    ${CONNEXTDDS_DIR}/include/ndds)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/../resources/cmake")

if(UNIX AND NOT APPLE)
    set(CMAKE_SKIP_RPATH OFF)
    set(CMAKE_BUILD_RPATH_USE_ORIGIN ON)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)
endif()

if(APPLE)
    set(CMAKE_SKIP_RPATH OFF)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)
endif()

string(TOUPPER ${CMAKE_BUILD_TYPE} RTI_LIB_BUILD_TYPE)

if(${RTI_LIB_BUILD_TYPE} STREQUAL "DEBUG")
    set(RTI_DEBUG_SUFFIX "d")
endif()

check_cxx_compiler_flag(-std=c++17 HAVE_FLAG_STD_CXX17)
check_cxx_compiler_flag(-std=c++14 HAVE_FLAG_STD_CXX14)

add_subdirectory(connextdds)
add_subdirectory(distlog)
add_subdirectory(request)
