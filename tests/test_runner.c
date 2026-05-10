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
extern void test_expand_path_result_is_always_null_terminated(void);
extern void test_expand_path_home_does_not_overflow(void);

extern void test_sync_folders_src_not_exists(void);
extern void test_sync_folders_copy_new_file(void);
extern void test_sync_folders_skip_older_file(void);
extern void test_sync_folders_atomic_success(void);
extern void test_sync_folders_atomic_cleanup_on_failure(void);
extern void test_sync_folders_atomic_defensive_cleanup(void);
extern void test_sync_folders_deletion_propagation_enabled(void);
extern void test_sync_folders_deletion_propagation_disabled(void);
extern void test_sync_folders_deletion_propagation_recursive(void);
extern void test_sync_folders_recursion(void);

extern void test_acquire_lock_success(void);
extern void test_acquire_lock_fail_already_running(void);
extern void test_release_lock_safe(void);

extern void test_is_process_running(void);
extern void test_launch_game_success(void);
extern void test_launch_cmdline_with_long_args(void);
extern void test_launch_cmdline_without_args(void);

int main(void) {
    UNITY_BEGIN();
    
    // Config
    RUN_TEST(test_expand_path_home);
    RUN_TEST(test_expand_path_relative);
    RUN_TEST(test_expand_path_no_prefix);
    RUN_TEST(test_load_config_missing_paths);
    RUN_TEST(test_expand_path_result_is_always_null_terminated);
    RUN_TEST(test_expand_path_home_does_not_overflow);
    
    // Sync
    RUN_TEST(test_sync_folders_src_not_exists);
    RUN_TEST(test_sync_folders_copy_new_file);
    RUN_TEST(test_sync_folders_skip_older_file);
    RUN_TEST(test_sync_folders_atomic_success);
    RUN_TEST(test_sync_folders_atomic_cleanup_on_failure);
    RUN_TEST(test_sync_folders_atomic_defensive_cleanup);
    RUN_TEST(test_sync_folders_deletion_propagation_enabled);
    RUN_TEST(test_sync_folders_deletion_propagation_disabled);
    RUN_TEST(test_sync_folders_deletion_propagation_recursive);
    RUN_TEST(test_sync_folders_recursion);
    
    // Lock
    RUN_TEST(test_acquire_lock_success);
    RUN_TEST(test_acquire_lock_fail_already_running);
    RUN_TEST(test_release_lock_safe);
    
    // Launch
    RUN_TEST(test_is_process_running);
    RUN_TEST(test_launch_game_success);
    RUN_TEST(test_launch_cmdline_with_long_args);
    RUN_TEST(test_launch_cmdline_without_args);
    
    return UNITY_END();
}
