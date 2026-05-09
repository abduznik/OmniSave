#include "omnisave.h"

void run_setup_wizard(const char *self_path, const char *arg_target) {
    printf("\n==========================================\n");
    printf("        OmniSave - Braindead Setup        \n");
    printf("==========================================\n");

    char target_exe[MAX_PATH];

    if (arg_target && strlen(arg_target) > 0) {
        strncpy(target_exe, arg_target, sizeof(target_exe) - 1);
        target_exe[sizeof(target_exe) - 1] = '\0';
        printf("Using provided Game Executable: %s\n", target_exe);
    } else {
        printf("Just paste the full path to the Game Executable:\n");
        printf("(e.g., /Volumes/Disk Pro/Games/GTA-V/Grand Theft Auto V/PlayGTAV.exe)\n");
        printf("(Tip: You can pass the path as an argument like './OmniSave /path/to/game.exe' to use Tab completion!)\n> ");
        if (!fgets(target_exe, sizeof(target_exe), stdin)) return;
        target_exe[strcspn(target_exe, "\r\n")] = 0;
    }

    // Isolate directory and filenames
    char game_dir[MAX_PATH] = ".";
    char base_name[MAX_PATH] = "game.exe";
    char original_exe[MAX_PATH];

    const char *last_slash = strrchr(target_exe, '/');
    const char *last_backslash = strrchr(target_exe, '\\');
    const char *slash = (last_slash > last_backslash) ? last_slash : last_backslash;

    if (slash) {
        size_t dir_len = slash - target_exe;
        strncpy(game_dir, target_exe, dir_len);
        game_dir[dir_len] = '\0';
        strcpy(base_name, slash + 1);
    } else {
        strcpy(base_name, target_exe);
    }

    char name_no_ext[MAX_PATH];
    char ext[64] = "";
    strcpy(name_no_ext, base_name);
    char *dot = strrchr(name_no_ext, '.');
    if (dot) {
        strcpy(ext, dot);
        *dot = '\0';
    }

    snprintf(original_exe, sizeof(original_exe), "%s/%s_original%s", game_dir, name_no_ext, ext);

    struct stat st;
    
    // Check if the user provided a valid source file
    if (stat(target_exe, &st) != 0) {
        // If the source doesn't exist, maybe it was ALREADY renamed successfully?
        if (stat(original_exe, &st) == 0) {
            printf("\n[Setup] Notice: '%s' already exists, and original is gone.\n", original_exe);
            printf("[Setup] Skipping rename to protect your real game executable.\n");
        } else {
            printf("\n[Setup] Error: The game executable '%s' does NOT exist!\n", target_exe);
            printf("Please check the path and spelling.\n");
            return;
        }
    } else {
        // Source exists. Does the original_exe already exist?
        if (stat(original_exe, &st) == 0) {
            printf("\n[Setup] Notice: '%s' already exists.\n", original_exe);
            printf("[Setup] Skipping rename to protect your real game executable.\n");
        } else {
            printf("\n[Setup] Renaming %s -> %s_original%s\n", base_name, name_no_ext, ext);
            if (rename(target_exe, original_exe) != 0) {
                printf("[Setup] Error: Failed to rename executable (permission denied or locked).\n");
                return;
            }
        }
    }

    printf("[Setup] Copying OmniSave wrapper to act as %s\n", base_name);
    
    // If running on Mac, we must copy the Windows .exe to the game folder so Wine can run it!
    char exe_payload[MAX_PATH];
    snprintf(exe_payload, sizeof(exe_payload), "%s.exe", self_path);
    FILE *src = fopen(exe_payload, "rb");
    if (!src) {
        src = fopen("OmniSave.exe", "rb"); // Try local directory
    }
    if (!src) {
        src = fopen(self_path, "rb"); // Fallback to itself
    }
    
    FILE *dest = fopen(target_exe, "wb");
    if (src && dest) {
        char buf[8192];
        size_t bytes;
        while ((bytes = fread(buf, 1, sizeof(buf), src)) > 0) {
            fwrite(buf, 1, bytes, dest);
        }
        fclose(src);
        fclose(dest);
        
        #ifndef _WIN32
        chmod(target_exe, 0755);
        #endif
    } else {
        printf("[Setup] Error: Failed to copy OmniSave executable.\n");
        if (src) fclose(src);
        if (dest) fclose(dest);
        return;
    }

    char ini_path[MAX_PATH];
    snprintf(ini_path, sizeof(ini_path), "%s/omnisave.ini", game_dir);
    printf("[Setup] Generating braindead config file at %s\n", ini_path);
    FILE *ini = fopen(ini_path, "w");
    if (ini) {
        fprintf(ini, "[OmniSave]\n");
        // FIX: Use ONLY the base name for the launch command. OmniSave runs in the game directory.
        // This prevents Wine from crashing due to macOS absolute paths like /Volumes/...
        fprintf(ini, "Launch_Command=\"%s_original%s\"\n", name_no_ext, ext);
        // Braindead mode: Just sync all of Documents and AppData into the game folder!
        fprintf(ini, "Host_Save_Path_1=~/Documents\n");
        fprintf(ini, "Portable_Save_Path_1=./portable_saves/Documents\n");
        fprintf(ini, "Host_Save_Path_2=~/AppData/Local\n");
        fprintf(ini, "Portable_Save_Path_2=./portable_saves/AppData_Local\n");
        fprintf(ini, "Host_Save_Path_3=~/AppData/Roaming\n");
        fprintf(ini, "Portable_Save_Path_3=./portable_saves/AppData_Roaming\n");
        fclose(ini);
        printf("\n[Setup] Success! You can now launch %s directly.\n", base_name);
        printf("[Setup] OmniSave will silently sync your entire Documents & AppData to the game's folder!\n");
    } else {
        printf("[Setup] Error: Failed to write omnisave.ini\n");
    }
}

int main(int argc, char *argv[]) {
    (void)argc;

    printf("==========================================\n");
    printf("         OmniSave - Portable Sync         \n");
    printf("==========================================\n");

    OmniConfig config;
    memset(&config, 0, sizeof(OmniConfig));

    if (!parse_config("omnisave.ini", &config)) {
        printf("[OmniSave] No 'omnisave.ini' found in current directory.\n");
        run_setup_wizard(argv[0], argc > 1 ? argv[1] : NULL);
        printf("\nPress ENTER to exit...");
        getchar();
        return 0;
    }

    // We found config, so we are in Proxy Mode.
    // 1. Hide the console window immediately on Windows so it's silent
    #ifdef _WIN32
    HWND hwnd = GetConsoleWindow();
    if (hwnd) ShowWindow(hwnd, SW_HIDE);
    #endif

    // 2. Redirect all output to omnisave.log
    freopen("omnisave.log", "a", stdout);
    freopen("omnisave.log", "a", stderr);

    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    char timestamp[64];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tm_info);

    printf("\n\n==========================================\n");
    printf("     OmniSave - Game Launched at %s\n", timestamp);
    printf("==========================================\n");

    // Pre-flight sync loop
    for (int i = 0; i < config.sync_path_count; i++) {
        if (strlen(config.sync_paths[i].host_path) == 0 || strlen(config.sync_paths[i].portable_path) == 0) continue;
        
        char resolved_host_path[MAX_PATH];
        resolve_dynamic_path(config.sync_paths[i].host_path, resolved_host_path, sizeof(resolved_host_path));
        
        printf("\n[OmniSave] Syncing Path pair %d\n", i);
        printf("[OmniSave] Target Save Path: %s\n", config.sync_paths[i].host_path);
        printf("[OmniSave] Discovered Host Path: %s\n", resolved_host_path);
        printf("[OmniSave] Portable Save Path: %s\n", config.sync_paths[i].portable_path);

        time_t host_time = get_latest_mtime(resolved_host_path);
        time_t portable_time = get_latest_mtime(config.sync_paths[i].portable_path);

        printf("[OmniSave] Host Save Modified Time: %ld\n", (long)host_time);
        printf("[OmniSave] Portable Save Modified Time: %ld\n", (long)portable_time);

        if (portable_time > host_time) {
            printf("[OmniSave] Portable save is newer. Syncing to Host...\n");
            if (create_backup(resolved_host_path)) {
                printf("[OmniSave] Backup successful.\n");
                if (copy_dir_recursive(config.sync_paths[i].portable_path, resolved_host_path)) {
                    printf("[OmniSave] Sync to Host successful.\n");
                } else {
                    printf("[OmniSave] Warning: Sync to Host encountered errors.\n");
                }
            } else {
                printf("[OmniSave] Warning: Backup failed. Skipping sync to Host to prevent data loss.\n");
            }
        } else {
            printf("[OmniSave] Host save is up-to-date or newer. Skipping pre-flight sync.\n");
        }
    }

    // Windows-specific Named Mutex to prevent infinite spawn loops
    #ifdef _WIN32
    HANDLE hMutex = CreateMutex(NULL, FALSE, "Global\\OmniSave_GTA5_Lock");
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        // This is a secondary instance spawned by the game
        if (hMutex) CloseHandle(hMutex);
        
        // Act as a dummy process until the main instance finishes
        while (true) {
            HANDLE hCheck = OpenMutex(MUTEX_ALL_ACCESS, FALSE, "Global\\OmniSave_GTA5_Lock");
            if (!hCheck) break; // Mutex is gone, main instance finished
            CloseHandle(hCheck);
            Sleep(1000);
        }
        return 0;
    }
    #else
    // Fallback for non-Windows
    if (access(".omnisave.lock", F_OK) == 0) {
        while (access(".omnisave.lock", F_OK) == 0) sleep(1);
        return 0;
    }
    FILE *lock = fopen(".omnisave.lock", "w");
    if (lock) { fprintf(lock, "running"); fclose(lock); }
    #endif

    // Launch game and wait
    launch_and_wait(config.launch_command);

    // Cleanup
    #ifdef _WIN32
    if (hMutex) {
        ReleaseMutex(hMutex);
        CloseHandle(hMutex);
    }
    #else
    remove(".omnisave.lock");
    #endif
    
    // Small delay to let any dummy instances finish sleeping before we exit
    #ifdef _WIN32
    Sleep(1000);
    #else
    sleep(1);
    #endif

    // Post-flight sync loop
    printf("\n[OmniSave] Post-flight check...\n");
    for (int i = 0; i < config.sync_path_count; i++) {
        if (strlen(config.sync_paths[i].host_path) == 0 || strlen(config.sync_paths[i].portable_path) == 0) continue;
        
        char resolved_host_path[MAX_PATH];
        resolve_dynamic_path(config.sync_paths[i].host_path, resolved_host_path, sizeof(resolved_host_path));
        
        time_t portable_time = get_latest_mtime(config.sync_paths[i].portable_path);
        time_t new_host_time = get_latest_mtime(resolved_host_path);
        
        printf("\n[OmniSave] Checking Path pair %d\n", i);
        printf("[OmniSave] Discovered Host Path: %s\n", resolved_host_path);
        printf("[OmniSave] New Host Save Modified Time: %ld\n", (long)new_host_time);

        if (new_host_time > portable_time) {
            printf("[OmniSave] Host save is newer. Syncing to Portable USB...\n");
            if (copy_dir_recursive(resolved_host_path, config.sync_paths[i].portable_path)) {
                printf("[OmniSave] Sync to Portable USB successful.\n");
            } else {
                printf("[OmniSave] Warning: Sync to Portable USB encountered errors.\n");
            }
        } else {
            printf("[OmniSave] Portable save is up-to-date. No post-flight sync needed.\n");
        }
    }

    printf("==========================================\n");
    printf("           OmniSave - Complete            \n");
    printf("==========================================\n");

    return 0;
}
