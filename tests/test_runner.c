#include "unity.h"
#include "mock_control.h"

void setUp(void) {
    mock_reset();
}

void tearDown(void) {
}

// Forward declarations of test functions
extern void test_expand_path_home(void);
extern void test_expand_path_relative(void);
extern void test_expand_path_no_prefix(void);
extern void test_load_config_missing_paths(void);

extern void test_sync_folders_src_not_exists(void);
extern void test_sync_folders_copy_new_file(void);
extern void test_sync_folders_skip_older_file(void);
extern void test_sync_folders_copy_newer_file(void);
extern void test_sync_folders_skip_apple_double(void);
extern void test_sync_folders_fails_on_copy_error(void);
extern void test_sync_folders_recursion(void);

extern void test_acquire_lock_success(void);
extern void test_acquire_lock_fail_already_running(void);
extern void test_release_lock_safe(void);

extern void test_is_process_running(void);
extern void test_launch_game_success(void);

int main(void) {
    UNITY_BEGIN();
    
    // Config
    RUN_TEST(test_expand_path_home);
    RUN_TEST(test_expand_path_relative);
    RUN_TEST(test_expand_path_no_prefix);
    RUN_TEST(test_load_config_missing_paths);
    
    // Sync
    RUN_TEST(test_sync_folders_src_not_exists);
    RUN_TEST(test_sync_folders_copy_new_file);
    RUN_TEST(test_sync_folders_skip_older_file);
    RUN_TEST(test_sync_folders_copy_newer_file);
    RUN_TEST(test_sync_folders_skip_apple_double);
    RUN_TEST(test_sync_folders_fails_on_copy_error);
    RUN_TEST(test_sync_folders_recursion);
    
    // Lock
    RUN_TEST(test_acquire_lock_success);
    RUN_TEST(test_acquire_lock_fail_already_running);
    RUN_TEST(test_release_lock_safe);
    
    // Launch
    RUN_TEST(test_is_process_running);
    RUN_TEST(test_launch_game_success);
    
    return UNITY_END();
}
