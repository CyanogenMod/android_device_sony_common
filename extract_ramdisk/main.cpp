/*
 * This binary extracts the ramdisk from a Sony ELF or ANDROID boot or
 * recovery image, gunzips it, checks to make sure that it is not a
 * stock recovery ramdisk, and then places it in the destination.
 * The goal is to use the FOTA partition to store a recovery image
 * and extract the ramdisk to be used during boot on Sony devices that
 * typically have a recovery-in-boot setup due to Sony disabling the
 * ability to boot the FOTA partition on unlocked devices. This binary
 * allows the recovery ramdisk to be separate from the boot partition
 * so that users can decide what recovery they want to have instead of
 * being stuck with whatever recovery their kernel or ROM maker decided
 * to include in the boot image.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *
 * The code was written from scratch by Dees_Troy dees_troy at
 * yahoo
 *
 * Copyright (c) 2013
 */

#include <stdlib.h>

int extract_ramdisk(int argc, const char** argv);

int main(int argc, const char** argv) {
	return extract_ramdisk(argc, argv);
}
