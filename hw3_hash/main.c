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

	// printf("\n");
	// setValue(h, "abasdc", 11); 			printHash(h);
	// setValue(h, "deLJHGf", 22); 		printHash(h);
	// setValue(h, "gh23465uhgfi", 33); 	printHash(h);
	// setValue(h, "jkladkjj", 44); 		printHash(h);
	// setValue(h, "mn243hgo", 55); 		printHash(h);
	// setValue(h, "mn243hgo", 10101); 	printHash(h);
	// setValue(h, ",limuknyjbthv", 423);
	// setValue(h, "dfsghjn ", 10567101);
	// setValue(h, " vcb cbdg", 789);
	// setValue(h, "][poiuh]", 345678765);
	// setValue(h, "ubjfm04-&", 890);
	// setValue(h, "-kgjfbv", 11111);
	// setValue(h, "dghyooo", 222222);
	// setValue(h, "aaaaaaaa", 33333);
	// setValue(h, "hhhhhhhh", 44444);
	// setValue(h, "tttttttt", 55555);
	// setValue(h, "kkkkkkkk", 66666);
	// setValue(h, "ppfoibrr", 77777);
	// setValue(h, "mmmmmnnn", 88888);
	// setValue(h, "cccd", 99999);
	// printHash(h);


	return 0;
}