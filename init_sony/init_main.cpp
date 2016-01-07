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
#include <stdlib.h>

#include <sys/mount.h>
#include <sys/stat.h>
#include <unistd.h>

#include "init_board.h"
#include "init_common.h"

// Main: executable
int main(int argc, char** __attribute__((unused)) argv)
{
    // Execution variables
    unsigned char i;
    bool recoveryBoot;
    char buffer_events[20];
    struct board_fota_config fota_config;

    // Generate boot log
    unlink(BOOT_TXT);
    write_string(BOOT_TXT, "SONY INIT");
    write_date(BOOT_TXT, true);

    // Delete /init symlink
    unlink("/init");

    // Create directories
    mkdir("/dev/block", 0755);
    mkdir("/dev/input", 0755);
    mkdir("/proc", 0555);
    mkdir("/sys", 0755);

    // Create device nodes
    mknod(DEV_BLOCK, S_IFBLK | 0600, makedev(179, 0));
    for (i = 0; i <= 12; ++i) {
        snprintf(buffer_events, sizeof(buffer_events), DEV_INPUT_EVENTS, i);
        mknod(buffer_events, S_IFCHR | 0600, makedev(DEV_INPUT_MAJOR, 64 + i));
    }
    mknod("/dev/null", S_IFCHR | 0666, makedev(1, 3));

    // Mount filesystems
    mount("proc", "/proc", "proc", 0, NULL);
    mount("sysfs", "/sys", "sysfs", 0, NULL);

    // Recovery boot detection
    const char* argv_boot_recovery[] = { EXEC_BOX, "grep",
            "warmboot=" WARMBOOT_RECOVERY, "/proc/cmdline", nullptr };
    recoveryBoot = (system_exec(argv_boot_recovery) == 0);

    // Keycheck introduction
    if (!recoveryBoot)
    {
        // Listen for volume keys
        const char* argv_keycheck[] = { EXEC_KEYCHECK, nullptr };
        pid_t keycheck_pid = system_exec_bg(argv_keycheck);

        // Board introduction animation
        board_introduce_keycheck();

        // Retrieve keycheck result
        recoveryBoot = (system_exec_kill(keycheck_pid) == KEYCHECK_RECOVERY);
    }

    // Boot to recovery
    if (recoveryBoot)
    {
        // Recovery boot
        write_string(BOOT_TXT, "RECOVERY BOOT", true);
        board_introduce_recovery();

        // Recovery ramdisk
        fota_config = board_fota_infos();
        if (fota_config.dev_major != -1) {
            mknod(fota_config.partition, S_IFBLK | 0600, makedev(
                    fota_config.dev_major, fota_config.dev_minor));
            mount("/", "/", NULL, MS_MGC_VAL | MS_REMOUNT, "");
            const char* argv_extract_elf[] = { "/sbin/extract_elf_ramdisk",
                    "-i", fota_config.partition, "-o",
                    "/sbin/ramdisk-recovery.cpio", "-t", "/", "-c", nullptr };
            system_exec(argv_extract_elf);
            const char* argv_ramdiskcpio[] = { EXEC_BOX, "cpio", "-i", "-F",
                    "/sbin/ramdisk-recovery.cpio", nullptr };
            system_exec(argv_ramdiskcpio);
        }
    }
    // Boot to Android
    else
    {
        // Android boot
        write_string(BOOT_TXT, "ANDROID BOOT", true);
        board_introduce_android();

        // Unpack Android ramdisk
        const char* argv_ramdiskcpio[] = { EXEC_BOX, "cpio", "-i", "-F",
                "/sbin/ramdisk.cpio", nullptr };
        system_exec(argv_ramdiskcpio);
    }

    // Finish init outputs
    board_finish_init();

    // Unmount filesystems
    umount("/proc");
    umount("/sys");

    // End of init
    const char* argv_rm_dev[] = { EXEC_BOX, "rm", "-fr", "/dev/*", nullptr };
    system_exec(argv_rm_dev);
    write_date(BOOT_TXT, true);

    // Launch ramdisk /init
    if (argc < 2) {
        const char* argv_init[] = { "/init", nullptr };
        system_exec(argv_init);
    }

    return 0;
}
