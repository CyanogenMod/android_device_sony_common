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

#ifndef __INIT_BOARD_COMMON_H__
#define __INIT_BOARD_COMMON_H__

#include "init_prototypes.h"

// Constants: file paths
#define BOOT_TXT "/boot.txt"
#define EXEC_KEYCHECK "/sbin/keycheck"
#define EXEC_TOYBOX "/sbin/toybox"
#define SBIN_CPIO_ANDROID "/sbin/ramdisk.cpio"
#define SBIN_CPIO_RECOVERY "/sbin/ramdisk-recovery.cpio"

// Constants: keycheck commands
#define KEYCHECK_RECOVERY_BOOT_ONLY (41 << 8)
#define KEYCHECK_RECOVERY_FOTA_BOOT (42 << 8)

// Class: init_board_common
class init_board_common
{
protected:
    init_board_common() {}
    virtual ~init_board_common() { }

public:
    // Board: start init execution
    virtual void start_init() { }

    // Board: introduction for keycheck
    virtual void introduce_keycheck() { }

    // Board: finalization of keycheck
    virtual void finish_keycheck(bool __attribute__((unused)) recoveryBoot) { }

    // Board: introduction for Android
    virtual void introduce_android() { }

    // Board: introduction for Recovery
    virtual void introduce_recovery() { }

    // Board: finish init execution
    virtual void finish_init() { }
};

#endif // __INIT_BOARD_COMMON_H__
