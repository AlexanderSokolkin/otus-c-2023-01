#pragma once
#include "constants.h"
#include <stdbool.h>


ArgsSet parseArgs(int argc, char* argv[]);
void printHelp();

bool printWeather(const char* t_region);

