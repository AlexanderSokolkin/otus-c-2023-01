#pragma once
#include <stdint.h>
#include <stddef.h>



extern const uint32_t 	EOCD_SIGNATURE;
extern const size_t		EOCDR_SIZE;

extern const uint32_t 	FHCD_SIGNATURE;
extern const size_t		FHCD_SIZE;

extern const uint16_t 	JPEG_START;
extern const uint16_t 	JPEG_END;



typedef struct {
	uint16_t	disk_number;
	uint16_t	start_disk_number;
	uint16_t	disk_entries;
	uint16_t	total_entries;
	uint32_t	size_CD;
	uint32_t	offset_CD;
	uint16_t	comment_len;
} EOCDR;

typedef struct {
	uint32_t	signature;
	uint16_t	version;
	uint16_t	version_needed;
	uint16_t	flag;
	uint16_t	compression;
	uint16_t	mod_time;
	uint16_t	mod_date;
	uint32_t	crc32;
	uint32_t	compressed_size;
	uint32_t	uncompressed_size;
	uint16_t	filename_len;
	uint16_t	extra_field_len;
	uint16_t	comment_len;
	uint16_t	disk_start;
	uint16_t	internal_attr;
	uint32_t	external_attr;
	uint32_t	local_file_header_offset;
} FHCD;

