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

#include <vector>
#include <string>
#include <algorithm>

#include <base/file.h>

#include "init_board_common.h"

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
void dir_unlink_r(std::string path_dir, bool rm_top, bool child, std::vector<std::string> exclusion_list) {
    struct stat cur_stat;
    std::vector<std::string> dir_content = get_dir_entries(path_dir);

    for(std::string file_name : dir_content) {
    	if(!contains_string(file_name, exclusion_list)) {
			if (stat(file_name.c_str(), &cur_stat) || S_ISDIR(cur_stat.st_mode)) {
				dir_unlink_r(file_name, rm_top, true, exclusion_list);
			} else {
				unlink(file_name.c_str());
			}
    	}
    }

    if ((child || rm_top) && (!contains_string(path_dir, exclusion_list) && get_dir_entries(path_dir).empty())) {
        rmdir(path_dir.c_str());
    }
}

std::vector<std::string> get_dir_entries(std::string path_dir) {
	std::vector<std::string> file_list;
    struct dirent* dir_r;
    std::string d_name_str;
    DIR* dir_p = opendir(path_dir.c_str());
    
	while (dir_p != NULL && (dir_r = readdir(dir_p)) != NULL) {
		d_name_str = std::string(dir_r->d_name);
		if (d_name_str == "." || d_name_str == "..") {
            continue;
        }
        
        std::string file_name = std::string(path_dir);
        if (file_name == "/" ) {
			file_name += std::string() + dir_r->d_name;
		} else {
			file_name += std::string() + "/" + dir_r->d_name;
		}
        file_list.push_back(file_name);
    }
    
    if (dir_p != NULL) {
        closedir(dir_p);
    }

    return file_list;
}

bool contains_string(std::string searchStr, std::vector<std::string> strList) {
    if(std::find(strList.begin(), strList.end(), searchStr) != strList.end()) {
		return true;
	}
    return false;
}
