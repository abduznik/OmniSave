#include "omnisave.h"
#include <ctype.h>

// Helper to trim whitespace
static char *trim_whitespace(char *str) {
    char *end;

    // Trim leading space
    while (isspace((unsigned char)*str)) str++;

    if (*str == 0) return str;

    // Trim trailing space
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;

    // Write new null terminator character
    end[1] = '\0';

    return str;
}

bool parse_config(const char *filename, OmniConfig *config) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("[OmniSave] Warning: Could not open config file '%s'. Using defaults.\n", filename);
        return false;
    }

    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        // Ignore comments and empty lines
        char *trimmed = trim_whitespace(line);
        if (trimmed[0] == '#' || trimmed[0] == ';' || trimmed[0] == '\0') {
            continue;
        }

        char *delimiter = strchr(trimmed, '=');
        if (delimiter) {
            *delimiter = '\0';
            char *key = trim_whitespace(trimmed);
            char *value = trim_whitespace(delimiter + 1);

            // Remove optional quotes from value
            if ((value[0] == '"' && value[strlen(value) - 1] == '"') ||
                (value[0] == '\'' && value[strlen(value) - 1] == '\'')) {
                value[strlen(value) - 1] = '\0';
                value++;
            }

            if (strcmp(key, "Target_Game_Exe") == 0) {
                strncpy(config->target_game_exe, value, MAX_PATH - 1);
            } else if (strncmp(key, "Host_Save_Path", 14) == 0) {
                int index = 0;
                if (strlen(key) > 15 && key[14] == '_') {
                    index = atoi(&key[15]);
                    // Shift to 0-based index if user starts at 1, but keep it simple: just use the number if it's within bounds
                    if (index > 0 && index < MAX_SYNC_PATHS) {
                         // We will map _1 to index 1, etc. index 0 is reserved for the un-suffixed key.
                         if (index >= config->sync_path_count) config->sync_path_count = index + 1;
                    } else {
                         index = 0; 
                    }
                } else {
                    if (config->sync_path_count == 0) config->sync_path_count = 1;
                }
                strncpy(config->sync_paths[index].host_path, value, MAX_PATH - 1);
            } else if (strncmp(key, "Portable_Save_Path", 18) == 0) {
                int index = 0;
                if (strlen(key) > 19 && key[18] == '_') {
                    index = atoi(&key[19]);
                    if (index > 0 && index < MAX_SYNC_PATHS) {
                         if (index >= config->sync_path_count) config->sync_path_count = index + 1;
                    } else {
                         index = 0;
                    }
                } else {
                    if (config->sync_path_count == 0) config->sync_path_count = 1;
                }
                strncpy(config->sync_paths[index].portable_path, value, MAX_PATH - 1);
            } else if (strcmp(key, "Launch_Command") == 0) {
                strncpy(config->launch_command, value, MAX_CMD - 1);
            }
        }
    }

    fclose(file);
    printf("[OmniSave] Config parsed successfully from '%s'.\n", filename);
    return true;
}
