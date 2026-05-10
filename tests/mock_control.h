#ifndef MOCK_CONTROL_H
#define MOCK_CONTROL_H

void mock_reset();
void mock_fs_add_file(const char* path, int is_dir, unsigned long long time);
void mock_process_set_running(const char* name, int is_running);
void mock_set_last_error(int err);

#endif
