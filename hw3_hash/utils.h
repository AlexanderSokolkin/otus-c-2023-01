#pragma once
#include "constants.h"
#include "hash.h"
#include <stdbool.h>


ArgsSet parseArgs(int argc, char* argv[]);
void printHelp();
bool checkFileContains(const char* t_filename);

bool readFile(const char* t_filename, hash t_hash);
