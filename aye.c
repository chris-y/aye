/* AYE
 * ZXAYEMUL metadata editor
 * (c) Chris Young 2021
 * https://www.unsatisfactorysoftware.co.uk
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <malloc.h>
#include <unistd.h>

#include "zxay_load.h"
#include "zxay_dump.h"
#include "zxay_peek.h"
#include "zxay_save.h"

int main(int argc, char **argv)
{
	int c;
	char *outfile = NULL;
	char *infile = NULL;
	bool debug = false;
	bool quiet = false;
	void *zxay;
	int i = 0;
	char numsongs;

	while ((c = getopt(argc, argv, "hdqo:")) != -1) {
		switch(c) {
			case 'h':
				printf("AYE 1.0\nZXAYEMUL metadata editor by Chris Young 2021\n\n");
				printf("aye [OPTIONS] [INPUT]\n\n");
				printf("Options:\n"
				"  -h This help\n"
				"  -q Quiet\n"
				"  -o Output file\n\n");
				return 0;
			break;
			case 'd':
				debug = true;
			break;
			case 'q':
				quiet = true;
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

	if(!quiet) {
		printf("AYE 1.0\nZXAYEMUL metadata editor by Chris Young 2021\n\n");
		printf("File: %s\n", infile);
	}

	if(zxay = zxay_load(infile)) {

		if(!quiet) {
			printf("Author: %s\n", zxay_peek(zxay, ZXAY_AUTHOR));
			printf("Misc: %s\n", zxay_peek(zxay, ZXAY_MISC));

			numsongs = zxay_peek(zxay, ZXAY_SONGCOUNT)[0];

			printf("Tracks: %d\n", numsongs + 1);
		
			for(i = 0; i <= numsongs; i++) {
				printf("  %d: %s\n", i + 1, zxay_peek_song(zxay, i, ZXAY_SONG_NAME));
			}
		}
		
		if(debug) zxay_dump(zxay);

		if(outfile) zxay_save(zxay, outfile);

		zxay_free(zxay);
	}
	
	return 0;
}

