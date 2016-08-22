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

#include <stdlib.h>

#include <sys/mount.h>

#include "init_prototypes.h"

// Function: partition block mount to path
void partition_mount(const char* block_path, const char* mount_point,
        bool ro, bool fs_ext4, bool fs_f2fs)
{
    int rc = -1;

    if (block_path == NULL || mount_point == NULL) {
        return;
    }

    umount(block_path);
    umount(mount_point);

    if (fs_f2fs) {
        rc = mount(block_path, mount_point, "f2fs", ro ? MS_RDONLY : 0, NULL);
    }

    if (rc != 0 && fs_ext4) {
        mount(block_path, mount_point, "ext4", ro ? MS_RDONLY : 0, NULL);
    }
}
