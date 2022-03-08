/* AYE ZXAYEMUL metadata editor
 * by Chris Young 2021, 2022
 * https://www.unsatisfactorysoftware.co.uk
 */
 
/* ZXAY file structure */
 
#ifndef ZXAY_FILE_H
#define ZXAY_FILE_H 1

#include <stdint.h>

#include "zxay.h"

/* structure for storing data blocks */
struct zxay_datablks {
        uint8_t **data;
        uint32_t *len; /* if zero, use song+block below */
        uint32_t *song;
        uint32_t *block;
};

struct zxay_file {
	struct zxay_header *header;
	struct zxay_song **songsstruct;
	char *author;
	char *misc;
	char **songname;
	struct zxay_songdata **songdata;
	struct zxay_songptrs **songptrs;
	struct zxay_songblks **songblks;
	struct zxay_datablks **datablocks;
	int16_t off_songstruct;
	int16_t *off_songdata;
	int16_t *off_songblks;
	int16_t *songblkcount;
};

#endif
