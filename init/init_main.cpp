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
#include "init_prototypes.h"

// Main: executable
int main(int argc, char** __attribute__((unused)) argv)
{
    // Execution variables
    unsigned char i;
    bool recoveryBoot;
    int keycheckStatus;
    char buffer_events[20];
    init_board_device init_board;

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
    mknod(DEV_BLOCK_PATH, S_IFBLK | 0600,
            makedev(DEV_BLOCK_MAJOR, DEV_BLOCK_MINOR));
    for (i = 0; i <= 12; ++i)
    {
        snprintf(buffer_events, sizeof(buffer_events), DEV_INPUT_EVENTS, i);
        mknod(buffer_events, S_IFCHR | 0600,
              makedev(DEV_INPUT_MAJOR, DEV_INPUT_MINOR_BASE + i));
    }
    mknod("/dev/null", S_IFCHR | 0666, makedev(1, 3));

    // Mount filesystems
    mount("proc", "/proc", "proc", 0, NULL);
    mount("sysfs", "/sys", "sysfs", 0, NULL);

    // Additional board inits
    init_board.start_init();

    // Recovery boot detection
    recoveryBoot = file_contains(WARMBOOT_CMDLINE, WARMBOOT_RECOVERY);

    // Keycheck introduction
    if (!recoveryBoot)
    {
#if KEYCHECK_ENABLED
        // Listen for volume keys
        const char* argv_keycheck[] = { EXEC_KEYCHECK, nullptr };
        pid_t keycheck_pid = system_exec_bg(argv_keycheck);

        // Board keycheck introduction
        init_board.introduce_keycheck();

        // Retrieve keycheck result
        keycheckStatus = system_exec_kill(keycheck_pid, KEYCHECK_TIMEOUT);
        recoveryBoot = (keycheckStatus == KEYCHECK_RECOVERY_BOOT_ONLY ||
                keycheckStatus == KEYCHECK_RECOVERY_FOTA_BOOT);
#endif
    }
    else
    {
        // Direct boot to FOTA / Boot Recovery
        keycheckStatus = KEYCHECK_RECOVERY_FOTA_BOOT;
    }

    // Boot to Recovery
    if (recoveryBoot)
    {
        // Recovery boot
        write_string(BOOT_TXT, "RECOVERY BOOT", true);
        init_board.introduce_recovery();

        // FOTA Recovery importation
        if (DEV_BLOCK_FOTA_NUM != -1 &&
                keycheckStatus != KEYCHECK_RECOVERY_BOOT_ONLY)
        {
            write_string(BOOT_TXT, "RECOVERY FOTA " DEV_BLOCK_FOTA_PATH, true);
            mknod(DEV_BLOCK_FOTA_PATH, S_IFBLK | 0600,
                    makedev(DEV_BLOCK_MAJOR, DEV_BLOCK_FOTA_NUM));
            mount("/", "/", NULL, MS_MGC_VAL | MS_REMOUNT, "");
            const char* argv_extract_elf[] = { "", "-i", DEV_BLOCK_FOTA_PATH,
                    "-o", SBIN_CPIO_RECOVERY, "-t", "/",
                    FOTA_RAMDISK_CHECK ? "-c" : "-z" };
            extract_ramdisk(sizeof(argv_extract_elf) / sizeof(const char*),
                    argv_extract_elf);
        }

        // Recovery ramdisk
        if (file_exists(SBIN_CPIO_RECOVERY))
        {
            const char* argv_ramdiskcpio[] = { EXEC_TOYBOX, "cpio", "-i", "-F",
                    SBIN_CPIO_RECOVERY, nullptr };
            system_exec(argv_ramdiskcpio);
        }
    }
    // Boot to Android
    else
    {
        // Android boot
        write_string(BOOT_TXT, "ANDROID BOOT", true);
        init_board.introduce_android();

        // Unpack Android ramdisk
        if (file_exists(SBIN_CPIO_ANDROID))
        {
            const char* argv_ramdiskcpio[] = { EXEC_TOYBOX, "cpio", "-i", "-F",
                    SBIN_CPIO_ANDROID, nullptr };
            system_exec(argv_ramdiskcpio);
        }
    }

    // Finish init outputs
    init_board.finish_init();
    write_date(BOOT_TXT, true);

    // Unmount filesystems
    umount("/proc");
    umount("/sys");

    // Init normally without parameters
    if (argc < 2)
    {
        // Unlink /dev/*
        dir_unlink_r("/dev", false);

        // Launch ramdisk /init
        const char* argv_init[] = { "/init", nullptr };
        system_exec(argv_init);
    }

    return 0;
}
