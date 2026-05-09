#include "omnisave.h"

int main(int argc, char* argv[]) {
    Config cfg;
    char base_path[MAX_PATH_LEN];
    char ini_path[MAX_PATH_LEN];
    char log_path[MAX_PATH_LEN];

    // 1. Get Absolute Path of EXE and Set CWD
    GetModuleFileNameA(NULL, base_path, MAX_PATH_LEN);
    char* last_slash = strrchr(base_path, '\\');
    if (last_slash) {
        *last_slash = '\0';
        SetCurrentDirectoryA(base_path);
        snprintf(ini_path, MAX_PATH_LEN, "%s\\%s", base_path, CONFIG_FILE);
        snprintf(log_path, MAX_PATH_LEN, "%s\\%s", base_path, LOG_FILE);
    } else {
        strcpy(ini_path, CONFIG_FILE);
        strcpy(log_path, LOG_FILE);
    }

    // 2. Initial Logging Redirect
    freopen(log_path, "a", stdout);
    freopen(log_path, "a", stderr);

    log_info("--- OmniSave Started ---");

    // 2. Lock Check
    if (!acquire_lock()) {
        MessageBoxA(NULL, "OmniSave is already running or cannot start.", "Error", MB_ICONERROR | MB_OK);
        return 1;
    }

    // 3. Load Config
    if (!load_config(&cfg, ini_path, base_path)) {
        MessageBoxA(NULL, "Failed to load config. Check omnisave.ini.", "Error", MB_ICONERROR | MB_OK);
        goto cleanup;
    }

    // 4. Pre-sync (Remote -> Local)
    log_info("Starting Pre-sync...");
    if (!sync_folders(cfg.remote_path, cfg.local_path)) {
        log_error("Pre-sync failed.");
    } else {
        log_info("Pre-sync completed successfully.");
    }

    // 5. Launch Game
    log_info("Launching game...");
    // <-- Pass cfg.launch_args as the second parameter
    launch_game(cfg.launch_command, cfg.launch_args, base_path); 

    // 6. Post-sync (Local -> Remote)
    log_info("Starting Post-sync...");
    if (!sync_folders(cfg.local_path, cfg.remote_path)) {
        log_error("Post-sync failed.");
    } else {
        log_info("Post-sync completed successfully.");
    }

cleanup:
    // 7. Unlock
    release_lock();

    log_info("--- OmniSave Finished ---");
    return 0;
}
