cmake_minimum_required(VERSION 3.15)
project(distlog)
include(CheckCXXCompilerFlag)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "$ENV{NDDSHOME}/resource/cmake")
find_package(
    RTIConnextDDS "5.3.1"
    REQUIRED
    COMPONENTS
        core
        distributed_logger
    )

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../pybind11 ${CMAKE_CURRENT_SOURCE_DIR}/../pybind11)

pybind11_add_module(
    distlog 
    MODULE
    NO_EXTRAS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/distlog.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/PyLogger.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/PyLoggerOptions.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/PyLogLevel.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/PyMessageParams.cpp"
)

set_target_properties(distlog PROPERTIES CXX_VISIBILITY_PRESET "default")

if (RTICONNEXTDDS_VERSION VERSION_GREATER_EQUAL "6.0.0")
    target_link_libraries(
        distlog PRIVATE
        ${CONNEXTDDS_EXTERNAL_LIBS}
        RTIConnextDDS::cpp2_api
        RTIConnextDDS::distributed_logger_c)
else()
    string(TOUPPER ${CMAKE_BUILD_TYPE} RTI_LIB_BUILD_TYPE)
    target_link_libraries(distlog PRIVATE
        ${CONNEXTDDS_EXTERNAL_LIBS}
        ${CONNEXTDDS_CPP2_API_LIBRARIES_${RTI_LIB_BUILD_TYPE}_SHARED}
        ${DISTRIBUTED_LOGGER_C_LIBRARIES_${RTI_LIB_BUILD_TYPE}_SHARED})

    target_include_directories(distlog PRIVATE ${CONNEXTDDS_INCLUDE_DIRS})

    if(CONNEXTDDS_DEFINITIONS MATCHES "-m64")
        string(REGEX REPLACE "-m64[ \t\r\n]*" "" CONNEXTDDS_DEFINITIONS "${CONNEXTDDS_DEFINITIONS}")
        target_compile_options(distlog PRIVATE "-m64")
    elseif(CONNEXTDDS_DEFINITIONS MATCHES "-m32")
        string(REGEX REPLACE "-m32[ \t\r\n]*" "" CONNEXTDDS_DEFINITIONS "${CONNEXTDDS_DEFINITIONS}")
        target_compile_options(distlog PRIVATE "-m32")
    endif()

    target_compile_definitions(distlog PRIVATE "${CONNEXTDDS_DEFINITIONS}")
endif()


target_include_directories(
    distlog PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../connextdds/include")

set_target_properties(distlog PROPERTIES PREFIX "${PYTHON_MODULE_PREFIX}"
                                         SUFFIX "${PYTHON_MODULE_EXTENSION}")

check_cxx_compiler_flag(-std=c++17 HAVE_FLAG_STD_CXX17)
check_cxx_compiler_flag(-std=c++14 HAVE_FLAG_STD_CXX14)
if (HAVE_FLAG_STD_CXX17 AND RTIConnextDDS_VERSION_MAJOR GREATER_EQUAL 6)
    set_target_properties(
        distlog
        PROPERTIES
            CXX_STANDARD 17
    )
elseif(HAVE_FLAG_STD_CXX14)
    set_target_properties(
        distlog
        PROPERTIES
            CXX_STANDARD 14
    )
else()
    set_target_properties(
        distlog
        PROPERTIES
            CXX_STANDARD 11
    )
endif()

target_compile_options(
    distlog
    PRIVATE
    "-fpic"
)

target_link_options(
    distlog
    PRIVATE
    "-fpic"
)
