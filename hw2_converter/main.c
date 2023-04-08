#include "convert.h"
#include "utils.h"
#include "constants.h"
#include <stdio.h>


int main(int argc, char* argv[])
{
	Codec codec;
	ArgsSet args = parseArgs(argc, argv, &codec);
	switch (args) {
		case Help:
			printHelp();
			break;
		case Convert:
			if (codec == InvalidCodec) {
				printf("unknown codec - %s\n", argv[2]);
				return 1;
			}
			if (!checkFileContains(argv[1])) {
				printf("Failed to open file - %s \n", argv[1]);
				return 2;
			}
			if (!convertToUTF8(argv[1], codec, argv[3])) {
				printf("Converting failed!\n");
				return 3;
			}
			break;
		default:
			printf("Invalid arguments!\n");
			printHelp();
			return 4;
	}

	return 0;
}
