cmake_minimum_required(VERSION 3.16)
project(ExultTools LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find ZLIB
find_package(ZLIB REQUIRED)

# Include directories
set(EXULT_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/..)
set(TOOL_INCLUDE_DIRS
    ${EXULT_ROOT}
    ${EXULT_ROOT}/headers
    ${EXULT_ROOT}/files
    ${EXULT_ROOT}/files/zip
    ${EXULT_ROOT}/conf
    ${CMAKE_CURRENT_BINARY_DIR}
)

# Generate minimal config.h for tools
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/config.h
"#ifndef CONFIG_H
#define CONFIG_H
#define HAVE_CONFIG_H 1
#define PACKAGE \"exult-tools\"
#define VERSION \"1.10.0\"
#define HAVE_STDINT_H 1
#define HAVE_UNISTD_H 1
#define HAVE_ZIP_SUPPORT 1
#define EXULT_DATADIR \"data\"
#endif
")

# Files library sources needed for tools
set(FILES_SOURCES
    ${EXULT_ROOT}/files/Flex.cc
    ${EXULT_ROOT}/files/Flat.cc
    ${EXULT_ROOT}/files/IFF.cc
    ${EXULT_ROOT}/files/Table.cc
    ${EXULT_ROOT}/files/U7file.cc
    ${EXULT_ROOT}/files/U7fileman.cc
    ${EXULT_ROOT}/files/U7obj.cc
    ${EXULT_ROOT}/files/crc.cc
    ${EXULT_ROOT}/files/listfiles.cc
    ${EXULT_ROOT}/files/utils.cc
    ${EXULT_ROOT}/files/zip/unzip.cc
    ${EXULT_ROOT}/files/zip/zip.cc
)

# Conf library sources
set(CONF_SOURCES
    ${EXULT_ROOT}/conf/Configuration.cc
    ${EXULT_ROOT}/conf/XMLEntity.cc
)

# Common sources
set(COMMON_SOURCES
    ${EXULT_ROOT}/istring.cc
)

# Build expack tool
add_executable(expack
    expack.cc
    ${FILES_SOURCES}
    ${CONF_SOURCES}
    ${COMMON_SOURCES}
)

target_include_directories(expack PRIVATE ${TOOL_INCLUDE_DIRS})
target_compile_definitions(expack PRIVATE HAVE_CONFIG_H EXULT_DATADIR="data")
target_link_libraries(expack PRIVATE ZLIB::ZLIB)

# Build ipack tool (image packer)
add_executable(ipack
    ipack.cc
    ${FILES_SOURCES}
    ${CONF_SOURCES}
    ${COMMON_SOURCES}
)

target_include_directories(ipack PRIVATE ${TOOL_INCLUDE_DIRS})
target_compile_definitions(ipack PRIVATE HAVE_CONFIG_H EXULT_DATADIR="data")
target_link_libraries(ipack PRIVATE ZLIB::ZLIB)

# Build textpack tool
add_executable(textpack
    textpack.cc
    ${FILES_SOURCES}
    ${CONF_SOURCES}
    ${COMMON_SOURCES}
)

target_include_directories(textpack PRIVATE ${TOOL_INCLUDE_DIRS})
target_compile_definitions(textpack PRIVATE HAVE_CONFIG_H EXULT_DATADIR="data")
target_link_libraries(textpack PRIVATE ZLIB::ZLIB)

message(STATUS "Exult Tools configured")
