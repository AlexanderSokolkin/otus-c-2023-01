#include "utils.h"
#include <stdio.h>
#include <string.h>


ArgsSet parseArgs(int argc, char* argv[])
{
	switch (argc) {
		case 2:
			if (strcmp(argv[1], "--help") == 0) {
				return Help;
			}
			return File;
		default:
			return InvalidArgs;
	}
}

void printHelp()
{
	const char* help = 	"this program counts the number of words in the transferred file.\n"
						"\tAvailable arguments:\n"
						"\t\t--help - print current help.";
	printf("%s\n", help);
}

bool checkFileContains(const char* t_filename)
{
	FILE* f = fopen(t_filename, "r");
	if (f == NULL) {
		return false;
	}
	fclose(f);
	return true;
}


bool readFile(const char* t_filename, hash t_hash)
{
	FILE* f = fopen(t_filename, "r");
	if (f == NULL) {
		return false;
	}

	char buf[1024];
	while (fscanf(f, "%s", buf) != EOF) {
		int* v = hashValue(t_hash, buf);
		if (v != NULL) {
			++(*v);
		}
	}

	fclose(f);
	return true;
}
