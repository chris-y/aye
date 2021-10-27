#include <stdint.h>
#include <stdbool.h>
#include <malloc.h>
#include <unistd.h>

#include "zxay_load.h"
#include "zxay_dump.h"

int main(int argc, char **argv)
{
	int c;
	char *outfile = NULL;
	char *infile = NULL;
	bool debug = false;
	void *zxay;

	printf("AYE 1.0\nZXAYEMUL metadata editor by Chris Young 2021\n\n");
		
	while ((c = getopt(argc, argv, "hdo:")) != -1) {
		switch(c) {
			case 'h':
				printf("aye [OPTIONS] [INPUT]\n\n");
				printf("Options:\n  -h This help\n  -o Output file\n\n");
				return 0;
			break;
			case 'd':
				debug = true;
			break;
			case 'o':
				outfile = optarg;
			break;
		}	
	}
	
	for(c = optind; c < argc; c++) {
		infile = argv[c];
	}
	
	if(infile == NULL) {
		printf("No file specified\n\n");
		return 0;
	}
	
	if(zxay = zxay_load(infile)) {

		if(debug) zxay_dump(zxay);

		//if(outfile) printf("%s\n", outfile);

		zxay_free(zxay);
	}
	
	return 0;
}

