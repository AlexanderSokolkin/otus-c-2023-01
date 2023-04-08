#pragma once
#include "constants.h"


ArgsSet parseArgs(int argc, char* argv[], Codec* t_codec);
void printHelp();
int checkFileContains(const char* t_filename);
