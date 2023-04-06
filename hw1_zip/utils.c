#define _DEFAULT_SOURCE
#include "utils.h"
#include "constants.h"
#include <stdint.h>
#include <endian.h>
#include <sys/stat.h>


#define ui16frombe(p) be16toh(*(uint16_t*)(p))
#define ui16fromle(p) le16toh(*(uint16_t*)(p))
#define ui32frombe(p) be32toh(*(uint32_t*)(p))
#define ui32fromle(p) le32toh(*(uint32_t*)(p))


static void readEOCDR(FILE* t_file, long t_end_offset, EOCDR* t_eocdr)
{
	fseek(t_file, t_end_offset, SEEK_END);
	uint8_t buf[EOCDR_SIZE];
	fread(buf, sizeof(buf[0]), EOCDR_SIZE, t_file);

	t_eocdr->disk_number = ui16fromle(buf);
	t_eocdr->start_disk_number = ui16fromle(buf + 2);
	t_eocdr->disk_entries = ui16fromle(buf + 4);
	t_eocdr->total_entries = ui16fromle(buf + 6);
	t_eocdr->size_CD = ui32fromle(buf + 8);
	t_eocdr->offset_CD = ui32fromle(buf + 12);
	t_eocdr->comment_len = ui16fromle(buf + 16);

	fseek(t_file, 0, SEEK_SET);
}

static void readFHCD(FILE* t_file, long t_offset, FHCD* t_fhcd)
{
	fseek(t_file, t_offset, SEEK_SET);
	uint8_t buf[FHCD_SIZE];
	fread(buf, sizeof(buf[0]), FHCD_SIZE, t_file);

	t_fhcd->signature = ui32fromle(buf);
	t_fhcd->version = ui16fromle(buf + 4);
	t_fhcd->version_needed = ui16fromle(buf + 6);
	t_fhcd->flag = ui16fromle(buf + 8);
	t_fhcd->compression = ui16fromle(buf + 10);
	t_fhcd->mod_time = ui16fromle(buf + 12);
	t_fhcd->mod_date = ui16fromle(buf + 14);
	t_fhcd->crc32 = ui32fromle(buf + 16);
	t_fhcd->compressed_size = ui32fromle(buf + 20);
	t_fhcd->uncompressed_size = ui32fromle(buf + 24);
	t_fhcd->filename_len = ui16fromle(buf + 28);
	t_fhcd->extra_field_len = ui16fromle(buf + 30);
	t_fhcd->comment_len = ui16fromle(buf + 32);
	t_fhcd->disk_start = ui16fromle(buf + 34);
	t_fhcd->internal_attr = ui16fromle(buf + 36);
	t_fhcd->external_attr = ui32fromle(buf + 38);
	t_fhcd->local_file_header_offset = ui32fromle(buf + 42);

	fseek(t_file, 0, SEEK_SET);
}

static int findEOCDR(FILE* t_file, size_t zip_size, EOCDR* t_eocdr)
{
	uint8_t buf[64];
	size_t total_offset = 0;

	while (total_offset < zip_size) {
		size_t offset = zip_size - total_offset;
		offset = offset < 64 ? offset : 64;
		fseek(t_file, -(total_offset + offset), SEEK_END);
		fread(buf, sizeof(buf[0]), offset, t_file);
		for (int i = (int)offset - 4; i >= 0; --i) {
			if (ui32frombe(buf + i) == EOCD_SIGNATURE) {
				if (t_eocdr != NULL) {
					readEOCDR(t_file, -(total_offset + offset - i -4), t_eocdr);
				}
				fseek(t_file, 0, SEEK_SET);
				return 1;
			}
		}
		total_offset += offset - (offset == 64 ? 3 : 0);
	}

	fseek(t_file, 0, SEEK_SET);
	return 0;
}




void printHelp()
{
	const char* help = 	"this program determines "
						"whether the file passed to it is "
						"a jpeg and zip splicing.\n"
						"\tinput format: isjpzip *filename*";
	printf("\n%s\n", help);
}

size_t fileSize(FILE* t_file)
{
	struct stat statBuf;
	if (fstat(fileno(t_file), &statBuf) != 0) {
		return 0;
	}

	return (size_t)statBuf.st_size;
}

size_t jpegSize(FILE* t_file)
{
	uint8_t buf[1024];
	if (!fread(buf, sizeof(buf[0]), 2, t_file) ||
		ui16frombe(buf) != JPEG_START)
	{
		return 0;
	}

	size_t jpeg_size = 2; // стартовая сигнатура JPEG_START занимает 2 байта
	size_t count = 0;
	do {
		count = fread(buf, sizeof(buf[0]), 1024, t_file);
		for (size_t i = 0; i < count - 1; ++i) {
			if (ui16frombe(buf + i) == JPEG_END) {
				fseek(t_file, 0, SEEK_SET);
				return jpeg_size + i + 2;
			}
		}
		jpeg_size += count - 1;
		fseek(t_file, -1, SEEK_CUR);
	} while(count);

	fseek(t_file, 0, SEEK_SET);
	return 0;
}

int isJpegZip(FILE* t_file, size_t t_jpeg_size)
{
	size_t f_size = fileSize(t_file);
	if (f_size <= t_jpeg_size) {
		return 0;
	}

	size_t zip_size = f_size - t_jpeg_size;
	return findEOCDR(t_file, zip_size, NULL);
}

void printZipFiles(FILE* t_file, size_t t_jpeg_size)
{
	size_t f_size = fileSize(t_file);
	if (f_size <= t_jpeg_size) {
		return;
	}

	size_t zip_size = f_size - t_jpeg_size;
	EOCDR eocdr;
	if (!findEOCDR(t_file, zip_size, &eocdr)) {
		return;
	}

	uint32_t offset = eocdr.offset_CD;
	for (uint16_t i = 0; i < eocdr.total_entries; ++i) {
		FHCD fhcd;
		readFHCD(t_file, t_jpeg_size + offset, &fhcd);
		fseek(t_file, t_jpeg_size + offset + FHCD_SIZE, SEEK_SET);
		char filename[fhcd.filename_len + 1];
		fread(filename, sizeof(filename[0]), fhcd.filename_len, t_file);
		filename[fhcd.filename_len] = '\0';
		printf("%d) %s\n", i + 1, filename);
		offset += FHCD_SIZE + fhcd.filename_len + fhcd.extra_field_len + fhcd.comment_len;
	}
}
