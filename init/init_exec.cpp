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

#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include "init_prototypes.h"

// Function: binary execution
int system_exec(const char* argv[])
{
    int status = -1;
    pid_t pid;

    pid = fork();
    if (pid == 0) {
        execv(argv[0], const_cast<char* const*>(&argv[0]));
        _exit(1);
    }

    waitpid(pid, &status, 0);
    return status;
}

// Function: binary background execution
pid_t system_exec_bg(const char* argv[])
{
    pid_t pid;

    pid = fork();
    if (pid == 0) {
        execv(argv[0], const_cast<char* const*>(&argv[0]));
    }

    return pid;
}

// Function: binary background kill with timeout
int system_exec_kill(pid_t pid, uint8_t timeout)
{
    int status = -1;
    pid_t _pid;
    time_t time_start, time_cur;

    time(&time_start);
    time_cur = time_start;

    do {
        _pid = waitpid(pid, &status, WNOHANG);
        if (_pid > 0) {
            return status;
        } else if (_pid == -1) {
            return -1;
        }
        time(&time_cur);
    } while (time_cur - time_start < timeout);

    kill(pid, SIGTERM);
    return 0;
}
