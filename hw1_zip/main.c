#include "utils.h"
#include <stdio.h>
#include <stdint.h>



int main(int argc, char* argv[])
{
	// проверяем количесвто параметров
	if (argc != 2) {
		printHelp();
		return 0;
	}

	// Пытаемся открыть файл
	FILE* f = fopen(argv[1], "rb+");
	if (!f) {
		printf("File '%s' not found!\n", argv[1]);
		return 0;
	}

	size_t js = jpegSize(f);
	if (isJpegZip(f, js)) {
		printf("File %s is jpegzip. Zip files:\n", argv[1]);
		printZipFiles(f, js);
	}
	else {
		printf("File %s is not jpegzip\n", argv[1]);
	}

	fclose(f);
	return 0;
}
