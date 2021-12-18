cmake_minimum_required(VERSION 3.0)
project(LOGGING)

set(LOGGING_VERSION_MAJOR 1)
set(LOGGING_VERSION_MINOR 0)
set(LOGGING_VERSION_PATCH 0)
set(LOGGING_VERSION ${LOGGING_VERSION_MAJOR}.${LOGGING_VERSION_MINOR}.${LOGGING_VERSION_PATCH})

set(LOGGING_VERSION_CMAKE ${PROJECT_BINARY_DIR}/logging-config-version.cmake)
configure_file(cmake/logging-config-version.cmake.in ${LOGGING_VERSION_CMAKE} @ONLY)

set(LOGGING_CONFIG_CMAKE ${PROJECT_BINARY_DIR}/logging-config.cmake)
configure_file(cmake/logging-config.cmake.in ${LOGGING_CONFIG_CMAKE} @ONLY)

################################################################################
# Library Configuration
################################################################################
message(STATUS "LOGGING_INCLUDE_PREFIX: ${LOGGING_INCLUDE_PREFIX}")

# THIS IS A HEADER FILE ONLY TARGET
add_library(logging INTERFACE)
target_include_directories(logging 
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include/logging-${LOGGING_VERSION}>
)

################################################################################
# Install Configuration
################################################################################
install(TARGETS logging 
    EXPORT logging
    DESTINATION lib/logging-${LOGGING_VERSION})
install(FILES Logging.h DESTINATION include/logging-${LOGGING_VERSION}/${LOGGING_INCLUDE_PREFIX})
install(FILES ${LOGGING_CONFIG_CMAKE} DESTINATION lib/logging-${LOGGING_VERSION})
install(FILES ${LOGGING_VERSION_CMAKE} DESTINATION lib/logging-${LOGGING_VERSION})
install(EXPORT logging DESTINATION lib/logging-${LOGGING_VERSION})

set(CPACK_PACKAGE_NAME "logging")
set(CPACK_SET_DESTDIR ON)
set(CPACK_GENERATOR "DEB")
set(CPACK_INSTALL_PREFIX "/usr/local")
set(CPACK_PACKAGE_VERSION ${LOGGING_VERSION})
set(CPACK_PACKAGE_CONTACT "doerthous@gmail.com")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "doerthous")
include(CPack)