#include "omnisave.h"

int main(int argc, char* argv[]) {
    Config cfg;
    char base_path[MAX_PATH_LEN];
    char ini_path[MAX_PATH_LEN];
    char log_path[MAX_PATH_LEN];

    p_get_module_path(base_path, MAX_PATH_LEN);
    char* last_slash = strrchr(base_path, '\\');
    if (!last_slash) last_slash = strrchr(base_path, '/');

    if (last_slash) {
        *last_slash = '\0';
        p_set_cwd(base_path);
        snprintf(ini_path, MAX_PATH_LEN, "%s\\%s", base_path, CONFIG_FILE);
        snprintf(log_path, MAX_PATH_LEN, "%s\\%s", base_path, LOG_FILE);
    } else {
        strcpy(ini_path, CONFIG_FILE);
        strcpy(log_path, LOG_FILE);
    }

    // Logging redirect
    freopen(log_path, "a", stdout);
    freopen(log_path, "a", stderr);

    log_info("--- OmniSave v%s Started ---", OMNISAVE_VERSION);

    if (!acquire_lock()) {
        p_show_error("Error", "OmniSave is already running or cannot start.");
        return 1;
    }

    if (!load_config(&cfg, ini_path, base_path)) {
        p_show_error("Error", "Failed to load config. Check omnisave.ini.");
        goto cleanup;
    }

    log_info("Starting Pre-sync...");
    if (!sync_folders(cfg.remote_path, cfg.local_path, 0)) {
        log_error("Pre-sync failed.");
    } else {
        log_info("Pre-sync completed successfully.");
    }

    log_info("Launching game...");
    launch_game(cfg.launch_command, cfg.launch_args, base_path); 

    log_info("Starting Post-sync...");
    if (!sync_folders(cfg.local_path, cfg.remote_path, 1)) {
        log_error("Post-sync failed.");
    } else {
        log_info("Post-sync completed successfully.");
    }

cleanup:
    release_lock();
    log_info("--- OmniSave Finished ---");
    return 0;
}
