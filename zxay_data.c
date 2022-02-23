/* AYE ZXAYEMUL metadata editor
 * by Chris Young 2021, 2022
 * https://www.unsatisfactorysoftware.co.uk
 */
 
/* ZXAY data routines */

#include "zxay_data.h"
#include "zxay_file.h"

uint32_t zxay_data_offset(void *zxayemul, int song, int block)
{
	int s;
	int b;
	uint32_t offset = 0;

	struct zxay_file *zxay = (struct zxay_file *)zxayemul;

	if(zxay->datablocks[song]->len[block] == 0) {
		return zxay_data_offset(zxay, zxay->datablocks[song]->song[block], zxay->datablocks[song]->block[block]);
	}

        for(s = 0; s <= song; s++) {
		for(b = 0; b <= block; b++) {
			offset += zxay->datablocks[s]->len[b];
		}
	}

	return offset - zxay->datablocks[song]->len[block];
}
