/* AYE ZXAYEMUL metadata editor
 * by Chris Young 2021, 2022
 * https://www.unsatisfactorysoftware.co.uk
 */

/* ZXAY merge (join) routines */

#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include "zxay_merge.h"
#include "zxay_file.h"

void *zxay_merge(void *zxayemul, void *zxayemulj)
{
	struct zxay_file *zxay = (struct zxay_file *)zxayemul;
	struct zxay_file *zxayj = (struct zxay_file *)zxayemulj;

	/* Create structure for merge */
	struct zxay_file *zxaym = calloc(sizeof(struct zxay_file), 1);
	zxaym->header = malloc(sizeof(struct zxay_header));
	memcpy(zxaym->header, zxay->header, sizeof(struct zxay_header));

	/* Copy author, take from file being merged if original doesn't have one */
	if(zxay->author) {
		zxaym->author = strdup(zxay->author);
	} else if (zxayj->author) {
		zxaym->author = strdup(zxayj->author);
	} else {
		zxaym->author = NULL;
	}

        /* Copy misc, take from file being merged if original doesn't have one */
        if(zxay->misc) {
                zxaym->misc = strdup(zxay->misc);
        } else if (zxayj->misc) {
                zxaym->misc = strdup(zxayj->misc);
        } else {
                zxaym->misc = NULL;
        }

	/* Add the 2nd file's number of songs to the first, add 1 as it starts at 0 */
	zxaym->header->NumOfSongs = zxay->header->NumOfSongs + zxayj->header->NumOfSongs + 1;

	/* Copy song structures */
	zxaym->songsstruct = calloc(sizeof(struct zxay_song), zxaym->header->NumOfSongs);
	memcpy(zxaym->songsstruct, zxay->songsstruct, sizeof(struct zxay_song) * (zxay->header->NumOfSongs + 1));
	memcpy(zxaym->songsstruct + zxay->header->NumOfSongs + 1, zxayj->songsstruct, sizeof(struct zxay_song) * (zxayj->header->NumOfSongs + 1));

	zxaym->songname = calloc(1, (zxaym->header->NumOfSongs + 1) * sizeof(char *));
	zxaym->songdata = calloc(1, (zxaym->header->NumOfSongs + 1) * sizeof(struct zxay_songdata *));
	zxaym->songptrs = calloc(1, (zxaym->header->NumOfSongs + 1) * sizeof(struct zxay_songptrs *));
	zxaym->songblks = calloc(1, (zxaym->header->NumOfSongs + 1) * sizeof(struct zxay_songblks *));
	zxaym->songblkcount = calloc(1, (zxaym->header->NumOfSongs + 1) * sizeof(int16_t *));
	zxaym->datablocks = calloc(1, (zxaym->header->NumOfSongs + 1) * sizeof(struct zxay_datablks *));

	int j = 0;
	struct zxay_file *src = zxay;

	for(int i = 0; i <= zxaym->header->NumOfSongs; i++) {
		if(i <= zxay->header->NumOfSongs) {
			src = zxay;
			j = i;
		} else {
			src = zxayj;
			j = i - zxay->header->NumOfSongs;
		}

		if(src->songname[j]) {
			zxaym->songname[i] = strdup(src->songname[j]);
		} else {
			zxaym->songname[i] = NULL;
		}

		zxaym->songblkcount[i] = src->songblkcount[j];

		memcpy(zxaym->songdata[i], src->songdata[j], sizeof(struct zxay_songdata));
		memcpy(zxaym->songptrs[i], src->songptrs[j], sizeof(struct zxay_songptrs));
		memcpy(zxaym->songblks[i], src->songblks[j], sizeof(struct zxay_songblks));
		memcpy(zxaym->datablocks[i], src->datablocks[j], sizeof(struct zxay_datablks));

	}

	zxay->free_data = false;
	zxayj->free_data = false;
	zxaym->free_data = true;

	return zxaym;
}
