#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>


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

void strToLower(char* t_str)
{
	int i = 0;
	while (t_str[i] != '\0') {
		t_str[i] = tolower(t_str[i]);
		++i;
	}
}

bool readFile(const char* t_filename, hash t_hash)
{
	FILE* f = fopen(t_filename, "r");
	if (f == NULL) {
		return false;
	}

	char sep[] = ",.!?:;*()-[]{}«»|\"\'\\/";
	char buf[64];
	while (fscanf(f, "%s", buf) != EOF) {
		char* istr = strtok(buf, sep);
		while (istr != NULL) {
			strToLower(istr);
			int* v = hashValue(t_hash, istr);
			if (v != NULL) {
				++(*v);
			}
			istr = strtok(NULL, sep);
		}
	}

	fclose(f);
	return true;
}
