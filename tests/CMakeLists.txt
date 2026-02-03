cmake_minimum_required(VERSION 3.16)

# Shared library tests

# Test framework (header only)
add_library(ultima_test_framework INTERFACE)
target_include_directories(ultima_test_framework INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# Test: Common Types
add_executable(test_common_types shared/test_common_types.cpp)
target_include_directories(test_common_types PRIVATE
    ${CMAKE_SOURCE_DIR}/shared
)
target_link_libraries(test_common_types PRIVATE ultima_test_framework)
add_test(NAME CommonTypesTests COMMAND test_common_types)

# Test: Exceptions
add_executable(test_exceptions shared/test_exceptions.cpp)
target_include_directories(test_exceptions PRIVATE
    ${CMAKE_SOURCE_DIR}/shared
)
target_link_libraries(test_exceptions PRIVATE ultima_test_framework)
add_test(NAME ExceptionsTests COMMAND test_exceptions)

# Custom test target
add_custom_target(shared_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS test_common_types test_exceptions
)
