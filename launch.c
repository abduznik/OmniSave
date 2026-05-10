#include "omnisave.h"

int is_process_running(const char* process_name) {
    return p_is_process_running(process_name);
}

int launch_game(const char* command, const char* args, const char* base_dir) {
    // Construct full path to executable
    char full_command[MAX_PATH_LEN];
    snprintf(full_command, MAX_PATH_LEN, "%s\\%s", base_dir, command);

    log_info("Executing Target: %s", full_command);
    log_info("Arguments: %s", args);
    log_info("Working Directory: %s", base_dir);

    if (p_launch_process(full_command, args, base_dir)) {
        log_info("Process created. Waiting for %s memory presence...", command);
        
        // Give the executable 10 seconds to fully unpack
        p_sleep(10000); 

        if (is_process_running(command)) {
            log_info("%s detected in memory. Holding session alive...", command);
            
            while (is_process_running(command)) {
                p_sleep(3000); 
            }
            log_info("%s has terminated.", command);
        } else {
            log_error("%s not found after 10 seconds. Did it crash?", command);
        }

        return 1;
    } else {
        log_error("Failed to execute launcher: %lu", p_get_last_error());
    }

    return 0;
}
