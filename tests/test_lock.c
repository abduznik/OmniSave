#include "unity.h"
#include "omnisave.h"
#include "mock_control.h"


void test_acquire_lock_success(void) {
    int result = acquire_lock();
    TEST_ASSERT_EQUAL(1, result);
}

void test_acquire_lock_fail_already_running(void) {
    mock_set_last_error(183); // ERROR_ALREADY_EXISTS
    int result = acquire_lock();
    TEST_ASSERT_EQUAL(0, result);
}

void test_release_lock_safe(void) {
    // Should not crash even if no lock held
    release_lock();
    
    acquire_lock();
    release_lock();
}
