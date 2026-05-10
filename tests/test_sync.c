#include "unity.h"
#include "omnisave.h"
#include "mock_control.h"


void test_sync_folders_src_not_exists(void) {
    int result = sync_folders("C:\\Src", "C:\\Dst", 0);
    TEST_ASSERT_EQUAL(0, result);
}

void test_sync_folders_copy_new_file(void) {
    mock_fs_add_file("C:\\Src\\save.dat", 0, 1000);
    int result = sync_folders("C:\\Src", "C:\\Dst", 0);
    TEST_ASSERT_EQUAL(1, result);
    TEST_ASSERT_TRUE(p_file_exists("C:\\Dst\\save.dat"));
}

void test_sync_folders_skip_older_file(void) {
    mock_fs_add_file("C:\\Src\\save.dat", 0, 1000);
    mock_fs_add_file("C:\\Dst\\save.dat", 0, 2000);
    int result = sync_folders("C:\\Src", "C:\\Dst", 0);
    TEST_ASSERT_EQUAL(1, result);
}

void test_sync_folders_atomic_success(void) {
    mock_fs_add_file("C:\\Src\\new.dat", 0, 1000);
    int result = sync_folders("C:\\Src", "C:\\Dst", 0);
    TEST_ASSERT_EQUAL(1, result);
    TEST_ASSERT_TRUE(p_file_exists("C:\\Dst\\new.dat"));
    TEST_ASSERT_FALSE(p_file_exists("C:\\Dst\\new.dat.omnitmp"));
}

void test_sync_folders_atomic_cleanup_on_failure(void) {
    mock_fs_add_file("C:\\Src\\fail.dat", 0, 1000);
    mock_set_last_error(5); // Fail copy
    
    int result = sync_folders("C:\\Src", "C:\\Dst", 0);
    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_FALSE(p_file_exists("C:\\Dst\\fail.dat"));
    TEST_ASSERT_FALSE(p_file_exists("C:\\Dst\\fail.dat.omnitmp"));
}

void test_sync_folders_atomic_defensive_cleanup(void) {
    mock_fs_add_file("C:\\Dst\\stale.dat.omnitmp", 0, 500);
    mock_fs_add_file("C:\\Src\\other.dat", 0, 1000);
    
    sync_folders("C:\\Src", "C:\\Dst", 0);
    TEST_ASSERT_FALSE(p_file_exists("C:\\Dst\\stale.dat.omnitmp"));
}

void test_sync_folders_deletion_propagation_enabled(void) {
    mock_fs_add_file("C:\\Src\\keep.dat", 0, 1000);
    mock_fs_add_file("C:\\Dst\\keep.dat", 0, 1000);
    mock_fs_add_file("C:\\Dst\\stale.dat", 0, 1000);
    
    sync_folders("C:\\Src", "C:\\Dst", 1);
    TEST_ASSERT_TRUE(p_file_exists("C:\\Dst\\keep.dat"));
    TEST_ASSERT_FALSE(p_file_exists("C:\\Dst\\stale.dat"));
}

void test_sync_folders_deletion_propagation_disabled(void) {
    mock_fs_add_file("C:\\Src\\keep.dat", 0, 1000);
    mock_fs_add_file("C:\\Dst\\stale.dat", 0, 1000);
    
    sync_folders("C:\\Src", "C:\\Dst", 0);
    TEST_ASSERT_TRUE(p_file_exists("C:\\Dst\\stale.dat"));
}

void test_sync_folders_deletion_propagation_recursive(void) {
    mock_fs_add_file("C:\\Src\\file.dat", 0, 1000);
    mock_fs_add_file("C:\\Dst\\stale_dir", 1, 0);
    mock_fs_add_file("C:\\Dst\\stale_dir\\file.dat", 0, 1000);
    
    sync_folders("C:\\Src", "C:\\Dst", 1);
    TEST_ASSERT_FALSE(p_directory_exists("C:\\Dst\\stale_dir"));
    TEST_ASSERT_FALSE(p_file_exists("C:\\Dst\\stale_dir\\file.dat"));
}

void test_sync_folders_recursion(void) {
    mock_fs_add_file("C:\\Src\\subdir", 1, 0);
    mock_fs_add_file("C:\\Src\\subdir\\file.txt", 0, 1000);
    int result = sync_folders("C:\\Src", "C:\\Dst", 0);
    TEST_ASSERT_EQUAL(1, result);
}
