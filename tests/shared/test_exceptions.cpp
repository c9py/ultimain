/*
 * test_exceptions.cpp - Tests for exceptions.h
 */

#include "../test_framework.h"
#include "exceptions.h"
#include <string>

// Test exult_exception basic functionality
bool test_exult_exception_basic() {
    exult_exception ex("Test error message");
    TEST_ASSERT_STRING_EQUAL("Test error message", ex.what());
    return true;
}

// Test exult_exception with source info
bool test_exult_exception_source_info() {
    exult_exception ex("Error", "test.cpp", 42);
    TEST_ASSERT_STRING_EQUAL("Error", ex.what());
    TEST_ASSERT_STRING_EQUAL("test.cpp", ex.sourcefile());
    TEST_ASSERT_EQUAL(42, ex.line());
    return true;
}

// Test quit_exception
bool test_quit_exception() {
    quit_exception ex(0);
    TEST_ASSERT_EQUAL(0, ex.get_result());
    TEST_ASSERT_STRING_EQUAL("Quit", ex.what());

    quit_exception ex2(1);
    TEST_ASSERT_EQUAL(1, ex2.get_result());

    return true;
}

// Test file_open_exception
bool test_file_open_exception() {
    file_open_exception ex("missing_file.txt", __FILE__, __LINE__);
    std::string msg = ex.what();
    TEST_ASSERT(msg.find("missing_file.txt") != std::string::npos);
    TEST_ASSERT(msg.find("Error opening") != std::string::npos);
    return true;
}

// Test file_write_exception
bool test_file_write_exception() {
    file_write_exception ex("readonly.txt", __FILE__, __LINE__);
    std::string msg = ex.what();
    TEST_ASSERT(msg.find("readonly.txt") != std::string::npos);
    TEST_ASSERT(msg.find("Error writing") != std::string::npos);
    return true;
}

// Test file_read_exception
bool test_file_read_exception() {
    file_read_exception ex("corrupt.dat", __FILE__, __LINE__);
    std::string msg = ex.what();
    TEST_ASSERT(msg.find("corrupt.dat") != std::string::npos);
    TEST_ASSERT(msg.find("Error reading") != std::string::npos);
    return true;
}

// Test wrong_file_type_exception
bool test_wrong_file_type_exception() {
    wrong_file_type_exception ex("data.bin", "FLEX", __FILE__, __LINE__);
    std::string msg = ex.what();
    TEST_ASSERT(msg.find("data.bin") != std::string::npos);
    TEST_ASSERT(msg.find("FLEX") != std::string::npos);
    return true;
}

// Test exception inheritance
bool test_exception_inheritance() {
    // All file exceptions should derive from file_exception
    try {
        throw file_open_exception("test.txt", __FILE__, __LINE__);
    } catch (const file_exception& e) {
        // Should catch as file_exception
        TEST_ASSERT(true);
    } catch (...) {
        TEST_ASSERT(false);
    }

    // All exceptions should derive from exult_exception
    try {
        throw file_read_exception("test.txt", __FILE__, __LINE__);
    } catch (const exult_exception& e) {
        // Should catch as exult_exception
        TEST_ASSERT(true);
    } catch (...) {
        TEST_ASSERT(false);
    }

    return true;
}

int main() {
    TEST_SUITE("Exceptions");

    RUN_TEST("exult_exception basic", test_exult_exception_basic);
    RUN_TEST("exult_exception source info", test_exult_exception_source_info);
    RUN_TEST("quit_exception", test_quit_exception);
    RUN_TEST("file_open_exception", test_file_open_exception);
    RUN_TEST("file_write_exception", test_file_write_exception);
    RUN_TEST("file_read_exception", test_file_read_exception);
    RUN_TEST("wrong_file_type_exception", test_wrong_file_type_exception);
    RUN_TEST("Exception inheritance", test_exception_inheritance);

    TEST_SUMMARY();
}
