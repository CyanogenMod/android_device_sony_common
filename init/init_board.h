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

#ifndef __INIT_BOARD_H__
#define __INIT_BOARD_H__

#include "init_board_common.h"
#include <init_sony/init_board_device.h>
#include "init_prototypes.h"

// Constants: devices controls
#ifndef DEV_BLOCK_PATH
#define DEV_BLOCK_PATH "/dev/block/mmcblk0"
#endif
#ifndef DEV_BLOCK_MAJOR
#define DEV_BLOCK_MAJOR 179
#endif
#ifndef DEV_BLOCK_MINOR
#define DEV_BLOCK_MINOR 0
#endif
#ifndef DEV_BLOCK_FOTA_NUM
#define DEV_BLOCK_FOTA_NUM -1
#endif
#ifndef DEV_BLOCK_FOTA_PATH
#define DEV_BLOCK_FOTA_PATH DEV_BLOCK_PATH "p" STR(DEV_BLOCK_FOTA_NUM)
#endif
#ifndef DEV_INPUT_EVENTS
#define DEV_INPUT_EVENTS "/dev/input/event%u"
#endif
#ifndef DEV_INPUT_MAJOR
#define DEV_INPUT_MAJOR 13
#endif
#ifndef DEV_INPUT_MINOR_BASE
#define DEV_INPUT_MINOR_BASE 64
#endif

// Constants: warmboot commands
#ifndef WARMBOOT_CMDLINE
#define WARMBOOT_CMDLINE "/proc/cmdline"
#endif
#ifndef WARMBOOT_RECOVERY
#define WARMBOOT_RECOVERY "warmboot=0x77665502"
#endif

// Constants: init configurations
#ifndef KEYCHECK_TIMEOUT
#define KEYCHECK_TIMEOUT 3
#endif

#endif // __INIT_BOARD_H__
