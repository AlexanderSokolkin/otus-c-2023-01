#include <stdio.h>
#include <stddef.h>
#include "hash.h"
#include "constants.h"
#include "utils.h"


int main(int argc, char* argv[])
{
	ArgsSet as = parseArgs(argc, argv);

	switch (as) {
		case Help:
			printHelp();
			break;
		case File:
		{
			hash h = createHash();
			if (h == NULL) {
				return 1;
			}
			if (!readFile(argv[1], h)) {
				removeHash(h);
				printf("Failed to read file!");
				return 1;
			}
			hashPrint(h);
			removeHash(h);
			break;
		}
		case InvalidArgs:
			printf("Invalid arguments!\n");
			printHelp();
			return 1;
	}

	return 0;
}