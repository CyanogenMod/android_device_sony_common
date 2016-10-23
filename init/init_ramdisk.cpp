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

#include "init_prototypes.h"

// Function: ramdisk contents cleaner
void ramdisk_clean_files(void)
{
    dir_unlink_r("/res", false);
    file_unlink_wildcard("/", "/fstab", "");
    file_unlink_wildcard("/", "", ".rc");
    unlink("/sbin/adbd");
    unlink("/sbin/healthd");
    unlink("/sbin/ueventd");
    unlink("/sbin/watchdogd");
    unlink("/cache");
    unlink("/charger");
    unlink("/d");
    unlink("/default.prop");
    unlink("/etc");
    unlink("/file_contexts.bin");
    unlink("/init");
    unlink("/init.real");
    unlink("/logo.rle");
    unlink("/property_contexts");
    unlink("/sdcard");
    unlink("/seapp_contexts");
    unlink("/selinux_version");
    unlink("/sepolicy");
    unlink("/service_contexts");
    unlink("/vendor");
}
