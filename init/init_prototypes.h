/*
 * Copyright (C) 2016 The CyanogenMod Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __INIT_PROTOTYPES_H__
#define __INIT_PROTOTYPES_H__

#include <unistd.h>

#include <vector>
#include <string>

// Macro: expansion to string
#define INT_TO_STR(x) #x
#define STR(x) INT_TO_STR(x)

// Macro: milliseconds sleep
#define msleep(_MILLIS_) usleep(_MILLIS_ * 1000)

// Prototypes: files and dirs controls
bool file_exists(const char* path);
bool file_empty(const char* path);
bool file_contains(const char* path, const char* needle);
void dir_unlink_r(std::string path_dir, bool rm_top = true, bool child = false, std::vector<std::string> exclusion_list = {});
std::vector<std::string> get_dir_entries(std::string path_dir);
bool contains_string(std::string cSearchStr, std::vector<std::string> strList);

// Prototypes: files outputs
void write_int(const char* path, int value);
void write_string(const char* path, const char* value, bool append = false);
void write_date(const char* path, bool append = false);

// Prototypes: binary executions
int system_exec(const char* argv[]);
pid_t system_exec_bg(const char* argv[]);
void system_exec_inline(const char* argv[]);
int system_exec_kill(pid_t pid, uint8_t timeout = 0);

// Prototype: elf ramdisk extraction
int extract_ramdisk(int argc, const char** argv);

#endif // __INIT_PROTOTYPES_H__
