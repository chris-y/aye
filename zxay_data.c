/* AYE ZXAYEMUL metadata editor
 * by Chris Young 2021, 2022
 * https://www.unsatisfactorysoftware.co.uk
 */
 
/* ZXAY data routines */

#include <stdbool.h>
#include <stdint.h>

#include "zxay_data.h"
#include "zxay_file.h"

static int32_t compare_block(uint8_t *a, uint8_t *b, uint32_t alen, uint32_t blen)
{
	uint32_t i = 0;
	int32_t offset = 0;
	bool start_found = false;

	while(i < alen) {
		if(a[i] != b[i]) return -1;
		i++;
	}

	return offset;
}

static uint16_t check_dupe(struct zxay_file *zxay, int song, int block)
{
	int s;
        int b;
	uint16_t found = 0;

	if(zxay->datablocks[song]->len[block] == 0) return false;

        for(s = song; s <= zxay->header->NumOfSongs; s++) {
                for(b = 0; b < zxay->songblkcount[s]; b++) {
			if((s == song) && (b <= block)) continue;
                        if(zxay->datablocks[song]->len[block] != zxay->datablocks[s]->len[b]) continue;
			if(compare_block(zxay->datablocks[song]->data[block], zxay->datablocks[s]->data[b],
				zxay->datablocks[song]->len[block], zxay->datablocks[s]->len[block]) >= 0) {
				zxay->datablocks[s]->len[b] = 0;
				zxay->datablocks[s]->song[b] = song;
				zxay->datablocks[s]->block[b] = block;
				found++;
			}
                }
        }

	return found;
}

uint16_t zxay_data_dedupe(void *zxayemul)
{
	struct zxay_file *zxay = (struct zxay_file *)zxayemul;
	int s;
	int b;
	uint16_t c = 0;
	uint16_t count = 0;

	for(s = 0; s <= zxay->header->NumOfSongs; s++) {
		for(b = 0; b < zxay->songblkcount[s]; b++) {
			if(c = check_dupe(zxay, s, b)) count += c;
		}
	}

	return count;
}

uint32_t zxay_data_offset(void *zxayemul, int song, int block)
{
	int s;
	int b;
	uint32_t offset = 0;

	struct zxay_file *zxay = (struct zxay_file *)zxayemul;

	if(zxay->datablocks[song]->len[block] == 0) {
		//printf("%d:%d len 0, returning offset of %d:%d\n", song, block, zxay->datablocks[song]->song[block], zxay->datablocks[song]->block[block]);
		return zxay_data_offset(zxay, zxay->datablocks[song]->song[block], zxay->datablocks[song]->block[block]);
	}

        for(s = 0; s <= song; s++) {
		for(b = 0; b < zxay->songblkcount[s]; b++) {
			if((s == song) && (b >= block)) break;
			offset += zxay->datablocks[s]->len[b];
		}
	}

	return offset;
}
