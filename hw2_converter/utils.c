#include "utils.h"
#include <string.h>
#include <stdio.h>



ArgsSet parseArgs(int argc, char* argv[], Codec* t_codec)
{
	*t_codec = InvalidCodec;
	switch (argc) {
		case 2:
			return strcmp(argv[1], "--help") == 0 ? Help : InvalidArgs;
		case 4:
			if (strcmp(argv[2], "cp1251") == 0 || strcmp(argv[2], "1") == 0) {
				*t_codec = CP1251;
				return Convert;
			}
			else if (strcmp(argv[2], "koi8") == 0 || strcmp(argv[2], "2") == 0) {
				*t_codec = KOI8;
				return Convert;
			}
			else if (strcmp(argv[2], "iso88595") == 0 || strcmp(argv[2], "3") == 0) {
				*t_codec = ISO88595;
				return Convert;
			}
			else {
				return InvalidArgs;
			}
		default:
			return InvalidArgs;
	}
}

void printHelp()
{
	const char* help = 	"this program converts source text file from encoding"
						"CP-1251, KOI-8, ISO-8859-5 to UTF-8.\n"
						"\tinput format: utfconvert *src file* *src file codec* *dst file*.\n"
						"\tavailable codec:\n"
						"\t\t1) cp1251 or 1\n"
						"\t\t2) koi8 or 2\n"
						"\t\t3) iso88595 or 3\n"
						"\tavailable options:\n"
						"\t\t--help - print current help.\n";
	printf("%s\n", help);
}

int checkFileContains(const char* t_filename)
{
	FILE* f = fopen(t_filename, "rb+");
	if (f == NULL) {
		return 0;
	}

	fclose(f);
	return 1;
}
