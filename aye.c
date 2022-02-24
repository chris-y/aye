/* AYE
 * ZXAYEMUL metadata editor
 * (c) Chris Young 2021, 2022
 * https://www.unsatisfactorysoftware.co.uk
 */
 
#define AYECOPYVER "AYE 1.2+git\nZXAYEMUL metadata editor by Chris Young 2021, 2022\n\n"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <malloc.h>
#include <unistd.h>

#include "zxay_load.h"
#include "zxay_dump.h"
#include "zxay_peek.h"
#include "zxay_poke.h"
#include "zxay_save.h"

int main(int argc, char **argv)
{
	int c;
	char *outfile = NULL;
	char *infile = NULL;
	char *newauthor = NULL;
	char *newmisc = NULL;
	char *newtitle = NULL;
	int song = 0;
	int debug = 0;
	bool quiet = false;
	bool updates = false;
	void *zxay;
	int i = 0;
	char numsongs;

	while ((c = getopt(argc, argv, "hqd:s:a:m:t:o:")) != -1) {
		switch(c) {
			case 'h':
				printf(AYECOPYVER);
				printf("aye [OPTIONS] [INPUT]\n\n");
				printf("Options:\n"
				"  -h This help\n"
				"  -q Quiet\n"
				"  -s Song number\n"
				"  -a New author\n"
				"  -m New misc\n"
				"  -t New title\n"
				"  -o Output file\n\n");
				return 0;
			break;
			case 'd':
				debug = atoi(optarg);
			break;
			case 'q':
				quiet = true;
			break;
			case 'a':
				newauthor = optarg;
			break;
			case 'm':
				newmisc = optarg;
			break;
			case 's':
				song = atoi(optarg);
			break;
			case 't':
				newtitle = optarg;
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
		printf("ERROR: No file specified\n\n");
		return 0;
	}

	if(!quiet) {
		printf(AYECOPYVER);
		printf("File: %s\n", infile);
	}

        if(newtitle && (song == 0)) {
                printf("\nWARNING: New title specified without -s, ignoring.\n\n");
                newtitle = NULL;
        }

	if(newauthor || newmisc || newtitle) updates = true;

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

		if(debug) zxay_dump(zxay, debug);
		if(updates && (!quiet)) printf("\nNew metadata:\n");

		if(newauthor) {
			if(!quiet) printf("  Author: %s\n", newauthor);
			zxay_poke(zxay, ZXAY_AUTHOR, newauthor);
		}
		if(newmisc) {
			if(!quiet) printf("  Misc: %s\n", newmisc);
			zxay_poke(zxay, ZXAY_MISC, newmisc);
		}
		if(newtitle) {
			song -= 1; /* First track is 0 */
			if(song <= numsongs) {
				if(!quiet) printf("  Track %d: %s\n", song + 1, newtitle);
				zxay_poke_track(zxay, song, ZXAY_SONG_NAME, newtitle);
			} else {
				printf("WARNING: Invalid song (%d/%d) specified.\n", song + 1, numsongs + 1);
			}
		}
		if(outfile) {
			if(!quiet) printf("\nOutput: %s\n", outfile);
			zxay_save(zxay, outfile, debug);
		} else {
			if(updates) {
				printf("WARNING: No output file specified; changes have not been saved.\n");
			}
		}

		zxay_free(zxay);
	}

	return 0;
}
