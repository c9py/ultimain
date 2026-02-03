/*
 * test_framework.h - Lightweight C++ unit testing framework
 *
 * A minimal testing framework for the Ultima Engines project
 */

#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <cmath>
#include <sstream>

namespace ultima_test {

// ANSI color codes
const char* const COLOR_RED    = "\x1b[31m";
const char* const COLOR_GREEN  = "\x1b[32m";
const char* const COLOR_YELLOW = "\x1b[33m";
const char* const COLOR_RESET  = "\x1b[0m";

// Test result structure
struct TestResult {
    std::string name;
    bool passed;
    std::string message;
};

// Test suite class
class TestSuite {
private:
    std::string suite_name;
    std::vector<TestResult> results;
    int tests_run = 0;
    int tests_passed = 0;
    int tests_failed = 0;

public:
    explicit TestSuite(const std::string& name) : suite_name(name) {
        std::cout << "\n" << COLOR_YELLOW << "=== Test Suite: " << name << " ===" << COLOR_RESET << "\n";
    }

    void run_test(const std::string& name, std::function<bool()> test_func) {
        std::cout << "  Running " << name << "...";
        std::cout.flush();
        tests_run++;

        try {
            bool result = test_func();
            if (result) {
                std::cout << COLOR_GREEN << " PASS" << COLOR_RESET << "\n";
                tests_passed++;
                results.push_back({name, true, ""});
            } else {
                std::cout << COLOR_RED << " FAIL" << COLOR_RESET << "\n";
                tests_failed++;
                results.push_back({name, false, "Test returned false"});
            }
        } catch (const std::exception& e) {
            std::cout << COLOR_RED << " FAIL (exception: " << e.what() << ")" << COLOR_RESET << "\n";
            tests_failed++;
            results.push_back({name, false, e.what()});
        } catch (...) {
            std::cout << COLOR_RED << " FAIL (unknown exception)" << COLOR_RESET << "\n";
            tests_failed++;
            results.push_back({name, false, "Unknown exception"});
        }
    }

    int get_failed_count() const { return tests_failed; }
    int get_passed_count() const { return tests_passed; }
    int get_total_count() const { return tests_run; }

    void print_summary() const {
        std::cout << "\n" << COLOR_YELLOW << "--- " << suite_name << " Summary ---" << COLOR_RESET << "\n";
        std::cout << "Tests run: " << tests_run << "\n";
        std::cout << COLOR_GREEN << "Passed: " << tests_passed << COLOR_RESET << "\n";
        if (tests_failed > 0) {
            std::cout << COLOR_RED << "Failed: " << tests_failed << COLOR_RESET << "\n";
        } else {
            std::cout << "Failed: 0\n";
        }
    }
};

// Global test registry
class TestRegistry {
private:
    std::vector<TestSuite*> suites;
    static TestRegistry* instance;

public:
    static TestRegistry& get() {
        static TestRegistry registry;
        return registry;
    }

    void add_suite(TestSuite* suite) {
        suites.push_back(suite);
    }

    int run_all() {
        int total_failed = 0;
        int total_passed = 0;
        int total_run = 0;

        for (auto* suite : suites) {
            total_failed += suite->get_failed_count();
            total_passed += suite->get_passed_count();
            total_run += suite->get_total_count();
        }

        std::cout << "\n" << COLOR_YELLOW << "==============================" << COLOR_RESET << "\n";
        std::cout << "Total Tests: " << total_run << "\n";
        std::cout << COLOR_GREEN << "Total Passed: " << total_passed << COLOR_RESET << "\n";
        if (total_failed > 0) {
            std::cout << COLOR_RED << "Total Failed: " << total_failed << COLOR_RESET << "\n";
        } else {
            std::cout << "Total Failed: 0\n";
        }
        std::cout << COLOR_YELLOW << "==============================" << COLOR_RESET << "\n";

        return total_failed > 0 ? 1 : 0;
    }
};

// Assertion helpers
inline bool assert_true(bool condition, const char* expr, const char* file, int line) {
    if (!condition) {
        std::cerr << "\n    " << COLOR_RED << "Assertion failed: " << expr
                  << " at " << file << ":" << line << COLOR_RESET;
    }
    return condition;
}

inline bool assert_equal(long long expected, long long actual, const char* file, int line) {
    if (expected != actual) {
        std::cerr << "\n    " << COLOR_RED << "Expected " << expected << ", got " << actual
                  << " at " << file << ":" << line << COLOR_RESET;
        return false;
    }
    return true;
}

inline bool assert_float_near(double expected, double actual, double epsilon, const char* file, int line) {
    if (std::fabs(expected - actual) > epsilon) {
        std::cerr << "\n    " << COLOR_RED << "Expected " << expected << " (+/- " << epsilon
                  << "), got " << actual << " at " << file << ":" << line << COLOR_RESET;
        return false;
    }
    return true;
}

inline bool assert_string_equal(const std::string& expected, const std::string& actual, const char* file, int line) {
    if (expected != actual) {
        std::cerr << "\n    " << COLOR_RED << "Expected \"" << expected << "\", got \"" << actual
                  << "\" at " << file << ":" << line << COLOR_RESET;
        return false;
    }
    return true;
}

template<typename T>
inline bool assert_not_null(T* ptr, const char* file, int line) {
    if (ptr == nullptr) {
        std::cerr << "\n    " << COLOR_RED << "Pointer is NULL at " << file << ":" << line << COLOR_RESET;
        return false;
    }
    return true;
}

} // namespace ultima_test

// Macros for easy test definition
#define TEST_ASSERT(cond) \
    if (!ultima_test::assert_true(cond, #cond, __FILE__, __LINE__)) return false

#define TEST_ASSERT_EQUAL(expected, actual) \
    if (!ultima_test::assert_equal(expected, actual, __FILE__, __LINE__)) return false

#define TEST_ASSERT_FLOAT_NEAR(expected, actual, epsilon) \
    if (!ultima_test::assert_float_near(expected, actual, epsilon, __FILE__, __LINE__)) return false

#define TEST_ASSERT_STRING_EQUAL(expected, actual) \
    if (!ultima_test::assert_string_equal(expected, actual, __FILE__, __LINE__)) return false

#define TEST_ASSERT_NOT_NULL(ptr) \
    if (!ultima_test::assert_not_null(ptr, __FILE__, __LINE__)) return false

#define TEST_SUITE(name) ultima_test::TestSuite suite(name)
#define RUN_TEST(name, func) suite.run_test(name, func)
#define TEST_SUMMARY() suite.print_summary(); return suite.get_failed_count() > 0 ? 1 : 0

#endif // TEST_FRAMEWORK_H
