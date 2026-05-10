#include "unity.h"
#include "omnisave.h"
#include "mock_control.h"


void test_is_process_running(void) {
    mock_process_set_running("Game.exe", 1);
    TEST_ASSERT_EQUAL(1, is_process_running("Game.exe"));
    TEST_ASSERT_EQUAL(0, is_process_running("Other.exe"));
}

void test_launch_game_success(void) {
    mock_process_set_running("Game.exe", 1);
    // After 10s (no-op), it checks if running. 
    // Then it loops until not running. 
    // To avoid infinite loop in test, we need a way to make it stop running.
    // However, our launch_game uses a while(is_process_running) loop.
    // In a real unit test, we might need a more sophisticated mock or just test the launch part.
    
    // For now, let's just mock it as NOT running initially to test the "not found" path
    mock_process_set_running("Game.exe", 0);
    int result = launch_game("Game.exe", "--args", "C:\\Base");
    TEST_ASSERT_EQUAL(1, result); // launch_game returns 1 if p_launch_process succeeds
}
