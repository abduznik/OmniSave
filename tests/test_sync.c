#include "unity.h"
#include "omnisave.h"
#include "mock_control.h"


void test_sync_folders_src_not_exists(void) {
    // No files added to mock_fs
    int result = sync_folders("C:\\Src", "C:\\Dst");
    TEST_ASSERT_EQUAL(0, result);
}

void test_sync_folders_copy_new_file(void) {
    mock_fs_add_file("C:\\Src\\save.dat", 0, 1000);
    // Dest doesn't exist (time 0)
    
    int result = sync_folders("C:\\Src", "C:\\Dst");
    TEST_ASSERT_EQUAL(1, result);
}

void test_sync_folders_skip_older_file(void) {
    mock_fs_add_file("C:\\Src\\save.dat", 0, 1000);
    mock_fs_add_file("C:\\Dst\\save.dat", 0, 2000); // Dest is newer
    
    int result = sync_folders("C:\\Src", "C:\\Dst");
    TEST_ASSERT_EQUAL(1, result);
    // Logic: should_copy = 0, but function returns success
}

void test_sync_folders_copy_newer_file(void) {
    mock_fs_add_file("C:\\Src\\save.dat", 0, 3000); // Src is newer
    mock_fs_add_file("C:\\Dst\\save.dat", 0, 2000);
    
    int result = sync_folders("C:\\Src", "C:\\Dst");
    TEST_ASSERT_EQUAL(1, result);
}

void test_sync_folders_skip_apple_double(void) {
    mock_fs_add_file("C:\\Src\\._hidden", 0, 1000);
    
    int result = sync_folders("C:\\Src", "C:\\Dst");
    TEST_ASSERT_EQUAL(1, result);
    // Should skip and not log/error
}

void test_sync_folders_fails_on_copy_error(void) {
    mock_fs_add_file("C:\\Src\\fail.dat", 0, 1000);
    mock_set_last_error(5); // Access Denied
    
    int result = sync_folders("C:\\Src", "C:\\Dst");
    // This is the "bug" test. It should fail (return 0) but previous version returned 1.
    // My refactor already fixed it to return all_success.
    TEST_ASSERT_EQUAL(0, result);
}

void test_sync_folders_recursion(void) {
    mock_fs_add_file("C:\\Src\\subdir", 1, 0);
    mock_fs_add_file("C:\\Src\\subdir\\file.txt", 0, 1000);
    
    int result = sync_folders("C:\\Src", "C:\\Dst");
    TEST_ASSERT_EQUAL(1, result);
}
