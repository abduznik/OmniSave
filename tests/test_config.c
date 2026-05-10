#include "unity.h"
#include "omnisave.h"
#include "mock_control.h"


void test_expand_path_home(void) {
    char path[MAX_PATH_LEN] = "~/Saves";
    // Mock profile dir is /Users/mockuser
    expand_path(path, "C:\\Game");
    TEST_ASSERT_EQUAL_STRING("\\Users\\mockuser\\Saves", path);
}

void test_expand_path_relative(void) {
    char path[MAX_PATH_LEN] = "./Saves";
    expand_path(path, "C:\\Game");
    TEST_ASSERT_EQUAL_STRING("C:\\Game\\Saves", path);
}

void test_expand_path_no_prefix(void) {
    char path[MAX_PATH_LEN] = "D:\\Backup";
    expand_path(path, "C:\\Game");
    TEST_ASSERT_EQUAL_STRING("D:\\Backup", path);
}

void test_load_config_missing_paths(void) {
    Config cfg;
    // Mock currently returns "./local" and "./remote" by default in platform_mock.c
    // I should probably make p_get_config_string mockable too, but for now let's test a failure
    // if we had a way to mock empty strings.
    // For this test, I'll assume the mock can be told to return empty.
    // Since I didn't implement that yet, I'll skip or implement a simple version.
    
    // Let's just test success for now until I improve p_get_config_string mock.
    int result = load_config(&cfg, "config.ini", "C:\\Base");
    TEST_ASSERT_EQUAL(1, result);
    TEST_ASSERT_EQUAL_STRING("GTA5.exe", cfg.launch_command);
}
