find_package(
    RTIConnextDDS "7.0.0"
    REQUIRED
    COMPONENTS
        core
        messaging_api
)

find_package(
    Python${RTI_PYTHON_MAJOR_VERSION}
    REQUIRED
    COMPONENTS
        Interpreter
        Development.Module
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
    set(REQUEST_INSTALL_RPATH "$ORIGIN:$ORIGIN/..")
endif()

if(APPLE)
    set(REQUEST_INSTALL_RPATH "@loader_path;@loader_path/..")
endif()

pybind11_add_module(
    _util_native 
    MODULE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/util.cpp"
)

set_target_properties(
    _util_native 
    PROPERTIES 
    CXX_VISIBILITY_PRESET "default"
    LIBRARY_OUTPUT_DIRECTORY "${RTI_REQUEST__UTIL_NATIVE_LIBRARY_OUTPUT_DIRECTORY}"
    LIBRARY_OUTPUT_DIRECTORY_DEBUG "${RTI_REQUEST__UTIL_NATIVE_LIBRARY_OUTPUT_DIRECTORY}"
    LIBRARY_OUTPUT_DIRECTORY_RELEASE "${RTI_REQUEST__UTIL_NATIVE_LIBRARY_OUTPUT_DIRECTORY}"
    RUNTIME_OUTPUT_DIRECTORY "${RTI_REQUEST__UTIL_NATIVE_LIBRARY_OUTPUT_DIRECTORY}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${RTI_REQUEST__UTIL_NATIVE_LIBRARY_OUTPUT_DIRECTORY}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${RTI_REQUEST__UTIL_NATIVE_LIBRARY_OUTPUT_DIRECTORY}"
    INSTALL_RPATH "${REQUEST_INSTALL_RPATH}"
    PREFIX "${PYTHON_MODULE_PREFIX}"
    SUFFIX "${PYTHON_MODULE_EXTENSION}"
)

find_library(
    rticonnextmsgcpp2_lib
    rticonnextmsgcpp2${RTI_DEBUG_SUFFIX}
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
    _util_native
    PRIVATE
    ${CONNEXTDDS_EXTERNAL_LIBS}
    connextdds
    ${rticonnextmsgcpp2_lib}
    ${nddscpp2_lib}
    ${nddsc_lib}
    ${nddscore_lib}
    pybind11::opt_size
)

if (RTI_LINK_OPTIMIZATIONS_ON)
    target_link_libraries(
        _util_native
        PRIVATE
        pybind11::thin_lto
    )
endif()

target_compile_definitions(
    _util_native
    PRIVATE "${CONNEXTDDS_DLL_EXPORT_MACRO}"
    PRIVATE "${CONNEXTDDS_COMPILE_DEFINITIONS}"
)

target_include_directories(
    _util_native
    PRIVATE ${CONNEXTDDS_INCLUDE_DIRS}
    "${CMAKE_CURRENT_SOURCE_DIR}/../connextdds/include"
)

if (HAVE_FLAG_STD_CXX17 AND RTIConnextDDS_VERSION_MAJOR GREATER_EQUAL 6)
    set_target_properties(
        _util_native
        PROPERTIES
            CXX_STANDARD 17
    )
elseif(HAVE_FLAG_STD_CXX14)
    set_target_properties(
        _util_native
        PROPERTIES
            CXX_STANDARD 14
    )
else()
    set_target_properties(
        _util_native
        PROPERTIES
            CXX_STANDARD 11
    )
endif()
