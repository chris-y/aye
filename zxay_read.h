/* AYE ZXAYEMUL metadata editor
 * by Chris Young 2021
 * https://www.unsatisfactorysoftware.co.uk
 */
 
/* ZXAY read routines */
 
#ifndef ZXAY_READ_H
#define ZXAY_READ_H 1

#include "zxay.h"

struct zxay_file {
	struct zxay_header *header;
	struct zxay_song *songsstruct;
	char *author;
	char *misc;
	char **songname;
	struct zxay_songdata **songdata;
	struct zxay_songpointers **songptrs;
	struct zxay_songdatablock **songdatablocks;
};

struct zxay_file *zxay_load(char *filename);
void zxay_free(struct zxay_file *zxay);

#endif
