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

#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "gelf.h"
#include "bootimg.h"
#include "zlib.h"

#define ELF_RAMDISK_LOCATION 2 // Ramdisk is the second file in the image
#define EER_DEFAULT_TMP "/tmp" // Scatch folder default location
#define EER_TMP_RAMDISK_CPIO "ramdisk.cpio" // temporary ramdisk cpio file name
#define EER_SEARCH_STRING "fota-ua" // String to search to determine if the
                                    // ramdisk is a stock Sony FOTA ramdisk
#define MEMORY_BUFFER_SIZE (const size_t)13*1024*1024 // Max size of uncompressed
                                                   // ramdisk (13 MB)
#ifndef PATH_MAX
#define PATH_MAX 255
#endif

typedef char* byte_p;

char input_filename[PATH_MAX], output_filename[PATH_MAX], tmp_dir[PATH_MAX];
int has_input = 0, has_output = 0, dont_unzip = 0, check_ramdisk = 0;
int arg_error = 0, ramdisk_loc = ELF_RAMDISK_LOCATION;

int path_exists(const char* path) {
	// Check to see if the path exists
	struct stat st;
	if (stat(path, &st) != 0)
		return 0;
	else
		return -1;
}

size_t uncompress_gzip_memory(const byte_p compressed_data,
	const size_t compressed_data_size, byte_p uncompressed_data,
	const size_t uncompressed_max_size) {
	z_stream zInfo = {0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	zInfo.avail_in = compressed_data_size;
	zInfo.total_in = compressed_data_size;
	zInfo.avail_out = uncompressed_max_size;
	zInfo.total_out = uncompressed_max_size;
	zInfo.next_in = compressed_data	;
	zInfo.next_out = uncompressed_data;
	size_t return_value = 0;
	unsigned long err = inflateInit2(&zInfo, 16 + MAX_WBITS); // zlib function

	if (err == Z_OK) {
		err = inflate( &zInfo, Z_FINISH); // zlib function
		if (err == Z_STREAM_END) {
			return_value = zInfo.total_out;
		} else {
			printf("gunzip error -- Err:inflate %i\n", err);
		}
	} else {
		printf("gunzip error -- Err:inflateInit2 %i\n", err);
	}
	inflateEnd(&zInfo);
	return(return_value);
}

int scan_file_for_data(char *filename, unsigned char *data, int data_size,
	unsigned long start_location, unsigned long *data_address) {
	FILE *pFile;
	unsigned long lSize;
	unsigned char *buffer, *last_needle = NULL;
	unsigned char needle[data_size];
	size_t result;
	int i, return_val = 0;

	pFile = fopen(filename, "rb");
	if(pFile==NULL){
		printf("Unabled to open file '%s'.\nFailed\n", filename);
		exit(-1);
	}

	fseek (pFile , 0 , SEEK_END);
	lSize = ftell(pFile);
	rewind(pFile);

	buffer = (unsigned char*)malloc(sizeof(unsigned char) * lSize);
	if(buffer == NULL){
		printf("Memory allocation error on '%s'!\nFailed\n", filename);
		exit(-1);
	}

	result = fread(buffer, 1, lSize, pFile);
	if (result != lSize) {
		printf("Error reading file '%s'\nFailed\n", filename);
		exit(-1);
	}

	for (i=0; i<data_size; i++) {
		needle[i] = *data;
		data++;
	}

	unsigned char *p = memmem(buffer + start_location, lSize - start_location,
		needle, data_size);

	if (!p) {
		return_val = -1;
	} else {
		*data_address = p - buffer + data_size;
	}

	fclose(pFile);
	free(buffer);
	return return_val;
}

void copy_file_part(const char* infile, const char* outfile,
	const unsigned long offset,	unsigned long file_size) {
	// Copies a file starting at location offset of size file_size
	// If file_size is 0 then this function will copy until the end of the file
	FILE *oFile; // output file
	FILE *iFile; // input file
	byte_p buffer, uncompressed_buffer;
	size_t result;

	if (path_exists(outfile) && unlink(outfile)) {
		printf("Unable to unlink '%s'\n", outfile);
		exit(-1);
	}

	// Open input file
	iFile = fopen(infile, "rb");
	if (iFile == NULL) {
		printf("Unable to open input file '%s'.\n", infile);
		exit(-1);
	}

	// Determine file size to copy
	if (file_size == 0) {
		fseek(iFile, 0, SEEK_END);
		file_size = ftell(iFile) - offset;
		rewind(iFile);
	}

	// Allocate memory for reading the ramdisk
	buffer = (unsigned char*)malloc(sizeof(unsigned char) * file_size);
	if (buffer == NULL) {
		printf("Unable to malloc memory for file copy.\n");
		fclose(iFile);
		exit(-1);
	}

	// Read the input
	if (offset != 0)
		fseek(iFile, offset, SEEK_SET);
	result = fread(buffer, 1, file_size, iFile);
	fclose(iFile);
	if (result != file_size) {
		printf("Error reading source file from '%s'\nFailed\n", infile);
		free(buffer);
		exit(-1);
	}

	if (!dont_unzip) {
		uncompressed_buffer = (byte_p) malloc(MEMORY_BUFFER_SIZE);
		if (uncompressed_buffer == NULL) {
			free(buffer);
			printf("Unable to malloc memory for gunzip.\nFailed\n");
			exit(-1);
		}
		size_t uncompressed_size;
		uncompressed_size = uncompress_gzip_memory(buffer, file_size,
			uncompressed_buffer, MEMORY_BUFFER_SIZE);
		free(buffer);
		if (uncompressed_size <= 0) {
			free(uncompressed_buffer);
			printf("Failed to gunzip\n");
			exit(-1);
		}
		printf("Original size: %lu, gunzipped: %lu\n", file_size,
			uncompressed_size);
		buffer = uncompressed_buffer;
		file_size = uncompressed_size;
	}

	// Open the output file
	oFile = fopen(outfile, "wb");
	if(oFile == NULL) {
		printf("Unabled to open output file '%s'.\nFailed\n", outfile);
		if (!dont_unzip)
			free(uncompressed_buffer);
		else
			free(buffer);
		exit(-1);
	}

	// Copy the file
	printf("Copying '%s' to '%s'\n", infile, outfile);
	result = fwrite(buffer, 1, file_size, oFile);
	if (!dont_unzip)
		free(uncompressed_buffer);
	else
		free(buffer);
	fclose(oFile);
	if (result != file_size) {
		printf("Write count does not match during copy file!\n");
		exit(-1);
	} else {
		printf("Finished writing file: '%s'\n", outfile);
	}
}

void extract_elf(const char* img_filename, unsigned long* offset,
	unsigned long* ramdisk_size) {
	int img_fd;
	Elf *e;
	GElf_Phdr phdr;

	if ((img_fd = open(input_filename, O_RDONLY, 0)) < 0) {
		printf("Unable to open '%s' for read.\n", input_filename);
		exit(-1);
	}

	if ((e = elf_begin(img_fd, ELF_C_READ, NULL)) == NULL) {
		printf("elf_begin failed.\n");
		close(img_fd);
		exit(-1);
	}

	if (elf_kind(e) != ELF_K_ELF) {
		printf("'%s' is not an ELF file.\n", img_filename);
		elf_end(e);
		close(img_fd);
		exit(-1);
	}

	if (gelf_getphdr(e, ramdisk_loc - 1, &phdr) != &phdr) {
		printf("Failed to get header %i\n", ramdisk_loc - 1);
		elf_end(e);
		close(img_fd);
		exit(-1);
	}
	*offset = (unsigned long) phdr.p_offset;
	*ramdisk_size = (unsigned long) phdr.p_filesz;
	// printf("Flags:       %ll\n", phdr.e_flags); Does not exist in Android's
	//                                             libelf
	elf_end(e);
	close(img_fd);
}

void extract_android(const char* img_filename, unsigned long* offset,
	unsigned long* ramdisk_size) {
	int img_fd;
	boot_img_hdr header;
	size_t result, header_size = sizeof(header);
	unsigned char* buffer;

	if (ramdisk_loc == 3) {
		close(img_fd);
		printf("Cannot extract rpm on ANDROID image format.\n");
		exit(-1);
	}

	buffer = (unsigned char*)malloc(sizeof(unsigned char) * header_size);
	if(buffer == NULL){
		printf("Memory allocation error for header!\n");
		exit(-1);
	}

	if ((img_fd = open(input_filename, O_RDONLY, 0)) < 0) {
		printf("Unable to open '%s' for read.\n", input_filename);
		exit(-1);
	}

	result = read(img_fd, buffer, header_size);
	close(img_fd);
	if (result != header_size) {
		free(buffer);
		printf("Error reading '%s' Android header.\n", img_filename);
		exit(-1);
	}
	memcpy(&header, buffer, header_size);
	free(buffer);

	if (!header.page_size) {
		printf("Error, page size is 0\n");
		exit(-1);
	}
	if (ramdisk_loc == ELF_RAMDISK_LOCATION) {
		if (!header.ramdisk_size) {
			printf("Error, ramdisk size is 0!\n");
			exit(-1);
		}
		*offset = (((header.kernel_size / header.page_size) + 1) * 
			header.page_size) + header.page_size;
		*ramdisk_size = header.ramdisk_size;
	} else {
		if (!header.kernel_size) {
			printf("Error, kernel size is 0!\n");
			exit(-1);
		}
		*offset = header.page_size;
		*ramdisk_size = header.kernel_size;
	}
}

void extract_ramdisk() {
	int img_fd, return_val = 0;
	char output[PATH_MAX];
	char command[4096], magic_buffer[4];
	size_t result, read_size = sizeof(magic_buffer);
	unsigned long offset, ramdisk_size;

	// Make sure that tmp_dir ends with a /
	if (tmp_dir[strlen(tmp_dir) - 1] != '/')
		strcat(tmp_dir, "/");

	if (elf_version(EV_CURRENT) == EV_NONE) {
		printf("ELF library initialization failed.\n");
		exit(-1);
	}

	if ((img_fd = open(input_filename, O_RDONLY, 0)) < 0) {
		printf("Unable to open '%s' for read.\n", input_filename);
		exit(-1);
	}

	result = read(img_fd, magic_buffer, read_size);
	if (result != read_size) {
		close(img_fd);
		printf("Error reading '%s' to check magic.\n", input_filename);
		exit(-1);
	}
	close(img_fd);

	if (strncmp(magic_buffer + 1, "ELF", 3) == 0) {
		printf("ELF format...\n");
		extract_elf(input_filename, &offset, &ramdisk_size);
	} else if (strncmp(magic_buffer, "ANDR", 4) == 0) {
		printf("ANDROID! format...\n");
		extract_android(input_filename, &offset, &ramdisk_size);
	} else {
		printf("Unknown magic of '%s' in '%s'.\n", magic_buffer,
			input_filename);
		exit(-1);
	}

	printf("Offset:      %lu\n", offset);
	printf("Size:        %lu\n", ramdisk_size);

	if (!check_ramdisk) {
		strcpy(output, output_filename);
	} else {
		strcpy(output, tmp_dir);
		strcat(output, EER_TMP_RAMDISK_CPIO);
		if (path_exists(output) && unlink(output)) {
			printf("Unable to unlink '%s'\n", output);
			exit(-1);
		}
	}

	copy_file_part(input_filename, output, offset,
		ramdisk_size);
	printf("Uncompressed ramdisk written to '%s'.\n", output);

	if (check_ramdisk) {
		printf("Checking ramdisk to ensure it is not a stock Sony recovery.");
		printf("\n  (Checking for %s)\n", EER_SEARCH_STRING);
		unsigned char needle[7] = EER_SEARCH_STRING;
		unsigned long fota_location;
		return_val = scan_file_for_data(output, needle, sizeof(needle), 0,
			&fota_location);
		if (return_val < 0) {
			printf("This is not a stock Sony recovery ramdisk.\n");
			if (rename(output, output_filename)) {
				printf("Failed to rename '%s' to '%s'.\n", output,
					output_filename);
				exit(-1);
			}
			printf("Ramdisk copied to '%s'\nDONE!\n", output_filename);
		} else {
			printf("This is a stock Sony recovery ramdisk.\n");
			printf("Ramdisk NOT copied to '%s'\n", output_filename);
		}
		unlink(output);
	}
}

void print_usage() {
	printf("ELF / Android Boot Image Ramdisk Extractor by Dees_Troy\n");
	printf("Usage:\n\nRequired:\n");
	printf(" -i <input filename>  Specifies the .elf or .img file to read");
	printf("the ramdisk from (required)\n");
	printf(" -o <output filename> Specifies the final target of the .cpio ");
	printf("file (required)\n\n");
	printf("Optional:\n");
	printf(" -t <target dir>      Specifies directory to use for scratch\n");
	printf("                      space (uses %s if not specified)\n",
		EER_DEFAULT_TMP);
	printf(" -d                   Do not gunzip\n");
	printf(" -c                   Check ramdisk for stock recovery (cannot\n");
	printf("                      be used with -d)\n\n");
	printf(" -0                   Extract kernel\n");
	printf(" -2                   Extract rpm (.elf format only)\n");
	printf("If a stock recovery ramdisk is found, the ramdisk will not be\n");
	printf("copied to the output filename.\n");
}

int main(int argc, char** argv) {
	int index;

	umask(0);

	if (argc < 5) {
		print_usage();
		exit(-1);
	}

	strcpy(tmp_dir, EER_DEFAULT_TMP);

	for (index = 1; index < argc; index++) {
		if (strlen(argv[index]) != 2) {
			arg_error = 1;
			index = argc;
		} else if (strncmp(argv[index], "-i", 2) == 0) {
			// Input filename
			if (index == argc - 1) {
				arg_error = 1;
				printf("Missing input filename.\n\n");
				index = argc;
			} else if (strlen(argv[index + 1]) > PATH_MAX - 1) {
				arg_error = 2;
				index = argc;
			} else {
				index++;
				strcpy(input_filename, argv[index]);
				if (!path_exists(input_filename)) {
					arg_error = 4;
					printf("Input filename '%s' does not exist.\n\n",
						input_filename);
					index = argc;
				}
			}
		} else if (strncmp(argv[index], "-o", 2) == 0) {
			// Output filename
			if (index == argc - 1) {
				arg_error = 1;
				printf("Missing output filename.\n\n");
				index = argc;
			} else if (strlen(argv[index + 1]) > PATH_MAX - 1) {
				arg_error = 2;
				index = argc;
			} else {
				index++;
				strcpy(output_filename, argv[index]);
			}
		} else if (strncmp(argv[index], "-t", 2) == 0) {
			// Temp folder specified
			if (index == argc - 1) {
				arg_error = 1;
				printf("Missing temp directory parameter.\n\n");
				index = argc;
			} else if (strlen(argv[index + 1]) > PATH_MAX - 1) {
				arg_error = 2;
				index = argc;
			} else {
				index++;
				strcpy(tmp_dir, argv[index]);
				if (!path_exists(tmp_dir)) {
					arg_error = 5;
					printf("Temp directory '%s' does not exist.\n\n",
						tmp_dir);
					index = argc;
				}
			}
		} else if (strncmp(argv[index], "-d", 2) == 0) {
			dont_unzip = 1;
		} else if (strncmp(argv[index], "-c", 2) == 0) {
			check_ramdisk = 1;
		} else if (strncmp(argv[index], "-0", 2) == 0) {
			ramdisk_loc = 1;
		} else if (strncmp(argv[index], "-2", 2) == 0) {
			ramdisk_loc = 3;
		} else {
			arg_error = 3;
			printf("Invalid argument '%s'.\n\n", argv[index]);
			index = argc;
		}
	}

	if (arg_error != 0) {
		print_usage();
		exit(-1);
	} else {
		extract_ramdisk();
		exit(0);
	}
}
