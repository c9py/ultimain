/*
 * test_common_types.cpp - Tests for common_types.h
 */

#include "../test_framework.h"
#include "common_types.h"
#include <cstdint>

// Test basic type definitions
bool test_type_sizes() {
    // Verify type sizes match expected values
    TEST_ASSERT_EQUAL(1, sizeof(uint8));
    TEST_ASSERT_EQUAL(2, sizeof(uint16));
    TEST_ASSERT_EQUAL(4, sizeof(uint32));
    TEST_ASSERT_EQUAL(1, sizeof(sint8));
    TEST_ASSERT_EQUAL(2, sizeof(sint16));
    TEST_ASSERT_EQUAL(4, sizeof(sint32));
    return true;
}

// Test type ranges
bool test_type_ranges() {
    uint8 u8_max = 255;
    uint8 u8_min = 0;
    TEST_ASSERT_EQUAL(255, u8_max);
    TEST_ASSERT_EQUAL(0, u8_min);

    uint16 u16_max = 65535;
    uint16 u16_min = 0;
    TEST_ASSERT_EQUAL(65535, u16_max);
    TEST_ASSERT_EQUAL(0, u16_min);

    sint8 s8_max = 127;
    sint8 s8_min = -128;
    TEST_ASSERT_EQUAL(127, s8_max);
    TEST_ASSERT_EQUAL(-128, s8_min);

    return true;
}

// Test type conversions
bool test_type_conversions() {
    uint32 big_val = 0x12345678;
    uint8 low_byte = static_cast<uint8>(big_val & 0xFF);
    TEST_ASSERT_EQUAL(0x78, low_byte);

    uint16 low_word = static_cast<uint16>(big_val & 0xFFFF);
    TEST_ASSERT_EQUAL(0x5678, low_word);

    return true;
}

int main() {
    TEST_SUITE("Common Types");

    RUN_TEST("Type sizes", test_type_sizes);
    RUN_TEST("Type ranges", test_type_ranges);
    RUN_TEST("Type conversions", test_type_conversions);

    TEST_SUMMARY();
}
