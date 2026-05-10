#include "unity.h"
#include "omnisave.h"
#include "mock_control.h"

void test_is_process_running(void) {
    mock_process_set_running("Game.exe", 1);
    TEST_ASSERT_EQUAL(1, is_process_running("Game.exe"));
    TEST_ASSERT_EQUAL(0, is_process_running("Other.exe"));
}

void test_launch_game_success(void) {
    mock_process_set_running("Game.exe", 0);
    int result = launch_game("Game.exe", "--args", "C:\\Base");
    TEST_ASSERT_EQUAL(1, result);
}

void test_launch_cmdline_with_long_args(void) {
    char long_args[MAX_CMDLINE_LEN];
    memset(long_args, 'A', MAX_CMDLINE_LEN - 1);
    long_args[MAX_CMDLINE_LEN - 500] = '\0'; // Still long but safe
    
    launch_game("Game.exe", long_args, "C:\\Base");
    
    char captured[MAX_CMDLINE_LEN];
    p_get_last_launched_command(captured);
    
    // Verify it starts with the quoted command
    TEST_ASSERT_EQUAL_INT(0, strncmp(captured, "\"C:\\Base\\Game.exe\"", 18));
    // Verify it contains the args
    TEST_ASSERT_NOT_NULL(strstr(captured, "AAAAA"));
}

void test_launch_cmdline_without_args(void) {
    launch_game("Game.exe", "", "C:\\Base");
    
    char captured[MAX_CMDLINE_LEN];
    p_get_last_launched_command(captured);
    
    TEST_ASSERT_EQUAL_STRING("\"C:\\Base\\Game.exe\"", captured);
}
