find_package(
    RTIConnextDDS "5.3.1"
    REQUIRED
    COMPONENTS
        core
        distributed_logger
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
    NO_EXTRAS
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

target_link_libraries(
    distlog
    PRIVATE
    ${CONNEXTDDS_EXTERNAL_LIBS}
    connextdds
    ${RTI_LINK_DIR}/${RTI_ARCH_LINK_PREFIX}rtidlc${RTI_DEBUG_SUFFIX}${RTI_ARCH_LINK_SUFFIX}
    ${RTI_LINK_DIR}/${RTI_ARCH_LINK_PREFIX}nddscpp2${RTI_DEBUG_SUFFIX}${RTI_ARCH_LINK_SUFFIX}
    ${RTI_LINK_DIR}/${RTI_ARCH_LINK_PREFIX}nddsc${RTI_DEBUG_SUFFIX}${RTI_ARCH_LINK_SUFFIX}
    ${RTI_LINK_DIR}/${RTI_ARCH_LINK_PREFIX}nddscore${RTI_DEBUG_SUFFIX}${RTI_ARCH_LINK_SUFFIX}
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

if (MSVC)
    add_definitions(/bigobj /MP${NUM_JOBS})
    target_compile_options(
        distlog
        PRIVATE
        "$<$<CONFIG:RELEASE>:-Os>"
    )
else()
    target_compile_options(
        distlog
        PRIVATE
        "-fPIC"
        "$<$<CONFIG:RELEASE>:-Os>"
        "$<$<CONFIG:RELEASE>:-g0>"
    )

    target_link_options(
        distlog
        PRIVATE
        "-fPIC"
    )

    add_custom_command(
        TARGET distlog
        POST_BUILD
        COMMAND $<$<CONFIG:Release>:strip>$<$<NOT:$<CONFIG:Release>>:echo> $<$<CONFIG:Release>:-x> $<$<CONFIG:Release>:$<TARGET_FILE:distlog>>
    )
endif()
