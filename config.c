#include "omnisave.h"
#include <stdarg.h>

void log_message(const char* level, const char* format, va_list args) {
    time_t now;
    time(&now);
    char* tstr = ctime(&now);
    tstr[strlen(tstr) - 1] = '\0'; 

    printf("[%s] [%s] ", tstr, level);
    vprintf(format, args);
    printf("\n");
    fflush(stdout);
}

void log_info(const char* format, ...) {
    va_list args;
    va_start(args, format);
    log_message("INFO", format, args);
    va_end(args);
}

void log_error(const char* format, ...) {
    va_list args;
    va_start(args, format);
    log_message("ERROR", format, args);
    va_end(args);
}

void expand_path(char* path, const char* base_dir) {
    char expanded[MAX_PATH_LEN] = {0};
    
    if (path[0] == '~') {
        char user_profile[MAX_PATH_LEN] = {0};
        if (p_get_user_profile_dir(user_profile, MAX_PATH_LEN)) {
            snprintf(expanded, MAX_PATH_LEN, "%s%s", user_profile, path + 1);
            for(int i = 0; expanded[i]; i++) {
                if(expanded[i] == '/') expanded[i] = '\\';
            }
            strncpy(path, expanded, MAX_PATH_LEN);
        }
    } 
    else if (strncmp(path, "./", 2) == 0) {
        snprintf(expanded, MAX_PATH_LEN, "%s\\%s", base_dir, path + 2);
        for(int i = 0; expanded[i]; i++) {
            if(expanded[i] == '/') expanded[i] = '\\';
        }
        strncpy(path, expanded, MAX_PATH_LEN);
    }
}

int load_config(Config* cfg, const char* ini_path, const char* base_dir) {
    log_info("Loading config from: %s", ini_path);

    p_get_config_string("OmniSave", "Launch_Command", "GTA5.exe", cfg->launch_command, MAX_PATH_LEN, ini_path);
    p_get_config_string("OmniSave", "Launch_Args", "", cfg->launch_args, MAX_PATH_LEN, ini_path); 
    p_get_config_string("OmniSave", "Local_Path", "", cfg->local_path, MAX_PATH_LEN, ini_path);
    p_get_config_string("OmniSave", "Remote_Path", "", cfg->remote_path, MAX_PATH_LEN, ini_path);

    expand_path(cfg->local_path, base_dir);
    expand_path(cfg->remote_path, base_dir);

    if (strlen(cfg->local_path) == 0 || strlen(cfg->remote_path) == 0) {
        log_error("Local_Path or Remote_Path missing in ini.");
        return 0;
    }

    log_info("Launch Command: %s", cfg->launch_command);
    log_info("Launch Args: %s", cfg->launch_args);
    log_info("Local Path: %s", cfg->local_path);
    log_info("Remote Path: %s", cfg->remote_path);

    return 1;
}
