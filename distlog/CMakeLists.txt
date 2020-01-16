cmake_minimum_required(VERSION 3.15)
project(distlog)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "$ENV{NDDSHOME}/resource/cmake")
find_package(
    RTIConnextDDS "6.0.0"
    REQUIRED
    COMPONENTS
        core
        distributed_logger
    )

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../pybind11 ${CMAKE_CURRENT_SOURCE_DIR}/../pybind11)

add_library(
    distlog 
    MODULE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/distlog.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/PyLogger.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/PyLoggerOptions.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/PyLogLevel.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/PyMessageParams.cpp"
)

target_link_libraries(distlog PRIVATE pybind11::module)

target_link_libraries(
    distlog PRIVATE
    ${CONNEXTDDS_EXTERNAL_LIBS}
    RTIConnextDDS::cpp2_api
    RTIConnextDDS::distributed_logger_c)


target_include_directories(
    distlog PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../connextdds/include")

set_target_properties(distlog PROPERTIES PREFIX "${PYTHON_MODULE_PREFIX}"
                                         SUFFIX "${PYTHON_MODULE_EXTENSION}")

set_target_properties(
    distlog
    PROPERTIES
        CXX_STANDARD 11
)

target_compile_options(
    distlog
    PRIVATE
    "-fpic"
    "-fvisibility=hidden"
    "-flto"
)

target_link_options(
    distlog
    PRIVATE
    "-fpic"
    "-flto"
)
