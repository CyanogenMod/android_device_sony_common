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

#include <stdio.h>

#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>

#include <base/file.h>
#include <string>

#include "init_prototypes.h"

// Function: file existence check
bool file_exists(const char* path)
{
    int fd, res = false;

    fd = open(path, O_RDONLY);
    if (fd >= 0) {
        res = true;
        close(fd);
    }

    return res;
}

// Function: empty file check
bool file_empty(const char* path)
{
    int fd, res = false;

    fd = open(path, O_RDONLY);
    if (fd >= 0) {
        res = (lseek(fd, 0, SEEK_END) == 0);
        close(fd);
    }

    return res;
}

// Function: search string in file
bool file_contains(const char* path, const char* needle)
{
    std::string file_content;
    android::base::ReadFileToString(path, &file_content);
    return (strstr(file_content.c_str(), needle) != NULL);
}

// Function: recursive director removal
void dir_unlink_r(const char* path_dir, bool rm_top, bool child)
{
    DIR* dir_p;
    struct dirent* dir_r;
    struct stat cur_stat;
    char cur_file[512] = { 0 };

    dir_p = opendir(path_dir);

    while (dir_p != NULL && (dir_r = readdir(dir_p)) != NULL) {
        if (dir_r->d_name[0] == '.') {
            continue;
        }
        snprintf(cur_file, sizeof(cur_file), "%s/%s", path_dir, dir_r->d_name);

        if (stat(cur_file, &cur_stat) || S_ISDIR(cur_stat.st_mode)) {
            dir_unlink_r(cur_file, rm_top, true);
        } else {
            unlink(cur_file);
        }
    }

    if (dir_p != NULL) {
        closedir(dir_p);
    }
    if (child || rm_top) {
        rmdir(path_dir);
    }
}
