find_package(
    RTIConnextDDS "5.3.1"
    REQUIRED
    COMPONENTS
        core
        distributed_logger
)

find_package(
    Python
    REQUIRED
    COMPONENTS
        Interpreter
        Development
)

find_package(
    pybind11
    REQUIRED
)

list(
    APPEND CMAKE_FIND_ROOT_PATH
    ${RTI_PLATFORM_DIR}
)

if(UNIX AND NOT APPLE)
    set(DISTLOG_INSTALL_RPATH "$ORIGIN:$ORIGIN/..")
endif()

if(APPLE)
    set(DISTLOG_INSTALL_RPATH "@loader_path;@loader_path/..")
endif()

pybind11_add_module(
    distlog 
    MODULE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/distlog.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/PyLogger.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/PyLoggerOptions.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/PyLogLevel.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/PyMessageParams.cpp"
)

set_target_properties(
    distlog 
    PROPERTIES 
    CXX_VISIBILITY_PRESET "default"
    LIBRARY_OUTPUT_DIRECTORY "${RTI_LOGGING_DISTLOG_LIBRARY_OUTPUT_DIRECTORY}"
    LIBRARY_OUTPUT_DIRECTORY_DEBUG "${RTI_LOGGING_DISTLOG_LIBRARY_OUTPUT_DIRECTORY}"
    LIBRARY_OUTPUT_DIRECTORY_RELEASE "${RTI_LOGGING_DISTLOG_LIBRARY_OUTPUT_DIRECTORY}"
    RUNTIME_OUTPUT_DIRECTORY "${RTI_LOGGING_DISTLOG_LIBRARY_OUTPUT_DIRECTORY}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${RTI_LOGGING_DISTLOG_LIBRARY_OUTPUT_DIRECTORY}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${RTI_LOGGING_DISTLOG_LIBRARY_OUTPUT_DIRECTORY}"
    INSTALL_RPATH "${DISTLOG_INSTALL_RPATH}"
    PREFIX "${PYTHON_MODULE_PREFIX}"
    SUFFIX "${PYTHON_MODULE_EXTENSION}"
)

find_library(
    rtidlc_lib
    rtidlc${RTI_DEBUG_SUFFIX}
)

find_library(
    nddscpp2_lib
    nddscpp2${RTI_DEBUG_SUFFIX}
)

find_library(
    nddsc_lib
    nddsc${RTI_DEBUG_SUFFIX}
)

find_library(
    nddscore_lib
    nddscore${RTI_DEBUG_SUFFIX}
)

target_link_libraries(
    distlog
    PRIVATE
    ${CONNEXTDDS_EXTERNAL_LIBS}
    connextdds
    ${rtidlc_lib}
    ${nddscpp2_lib}
    ${nddsc_lib}
    ${nddscore_lib}
    pybind11::thin_lto
    pybind11::opt_size
)

if (RTICONNEXTDDS_VERSION VERSION_GREATER_EQUAL "6.0.0")
    target_compile_definitions(
        distlog
        PRIVATE "${CONNEXTDDS_DLL_EXPORT_MACRO}"
        PRIVATE "${CONNEXTDDS_COMPILE_DEFINITIONS}")
else()
    if(CONNEXTDDS_DEFINITIONS MATCHES "-m64")
        string(REGEX REPLACE "-m64[ \t\r\n]*" "" CONNEXTDDS_DEFINITIONS "${CONNEXTDDS_DEFINITIONS}")
        target_compile_options(distlog PRIVATE "-m64")
    elseif(CONNEXTDDS_DEFINITIONS MATCHES "-m32")
        string(REGEX REPLACE "-m32[ \t\r\n]*" "" CONNEXTDDS_DEFINITIONS "${CONNEXTDDS_DEFINITIONS}")
        target_compile_options(distlog PRIVATE "-m32")
    endif()

    target_compile_definitions(
        distlog
        PRIVATE "${CONNEXTDDS_DEFINITIONS}"
        PRIVATE "${CONNEXTDDS_DLL_EXPORT_MACRO}")
endif()

target_include_directories(
    distlog
    PRIVATE ${CONNEXTDDS_INCLUDE_DIRS}
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../connextdds/include"
)

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
