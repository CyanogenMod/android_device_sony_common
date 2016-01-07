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

#include <fcntl.h>
#include <time.h>
#include <unistd.h>

#include "init_prototypes.h"

// Constants: buffer max length
#define CMD_MAX_LENGTH 80

// Global: Shared buffer
char buffer[CMD_MAX_LENGTH];

// Function: int to path
void write_int(const char* path, int value)
{
    int fd, bytes;

    fd = open(path, O_CREAT | O_RDWR);
    if (fd >= 0) {
        bytes = snprintf(buffer, sizeof(buffer), "%d\n", value);
        write(fd, buffer, bytes);
        close(fd);
    }
}

// Function: string to path
void write_string(const char* path, const char* value, bool append)
{
    int fd, bytes;

    fd = open(path, O_CREAT | O_RDWR | (append ? O_APPEND : 0));
    if (fd >= 0) {
        bytes = snprintf(buffer, sizeof(buffer), "%s\n", value);
        write(fd, buffer, bytes);
        close(fd);
    }
}

// Function: date to path
void write_date(const char* path, bool append)
{
    int fd, bytes;
    struct tm* time_info;
    time_t time_raw;

    fd = open(path, O_CREAT | O_RDWR | (append ? O_APPEND : 0));
    if (fd >= 0) {
        time(&time_raw);
        time_info = localtime(&time_raw);
        bytes = strftime(buffer, sizeof(buffer), "%a %b %e %H:%M:%S %Z %Y\n",
                time_info);
        write(fd, buffer, bytes);
        close(fd);
    }
}
