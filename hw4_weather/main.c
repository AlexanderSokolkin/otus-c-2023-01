#include "constants.h"
#include "utils.h"
#include <stdio.h>



int main(int argc, char* argv[])
{
	ArgsSet as = parseArgs(argc, argv);

	switch (as) {
		case Help:
			printHelp();
			break;
		case Region:
			if (!printWeather(argv[1])) {
				return 1;
			}
			break;
		case InvalidArgs:
		default:
			printf("Invalid arguments!\n");
			printHelp();
			return 1;
	}

	return 0;
}