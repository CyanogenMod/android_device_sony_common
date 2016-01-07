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

#include "init_common.h"

// Structure: board FOTA configuration
struct board_fota_config {
    char partition[25];
    int dev_major;
    int dev_minor;
};

// Prototypes: board specific functions
struct board_fota_config board_fota_infos();
void board_introduce_keycheck();
void board_introduce_android();
void board_introduce_recovery();
void board_led_brightness(unsigned char, unsigned char, unsigned char);
void board_led_color(unsigned char, unsigned char, unsigned char);
void board_vibrate(unsigned int);

#endif // __INIT_BOARD_H__
