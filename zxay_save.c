/* AYE ZXAYEMUL metadata editor
 * by Chris Young 2021, 2022
 * https://www.unsatisfactorysoftware.co.uk
 */
 
/* ZXAY save routines */

#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "zxay_data.h"
#include "zxay_dump.h"
#include "zxay_file.h"
#include "zxay_save.h"
#include "zxay_misc.h"

static void zxay_write(FILE *fp, uint8_t *structure, size_t size, int debug, int type)
{
	int i = 0;

	if(size > 0) {
		for(i = 0; i < size; i++) {
			fputc(structure[i], fp);
		}
	}

	if(type == ZXAY_S_DATA) {
		if(debug > 3) zxay_dump_struct(structure, size, type);
	} else {
		if(debug > 1) zxay_dump_struct(structure, size, type);
	}
}

static void zxay_put_int16(int16_t val, uint8_t *dest)
{
	dest[0] = (uint8_t)(val >> 8);
	dest[1] = (uint8_t)(val);
}

bool zxay_save(void *zxayemul, char *filename, int debug)
{
	FILE *fp;
	struct zxay_file *zxay = (struct zxay_file *)zxayemul;
	int i = 0;
	int b = 0;
	uint32_t squashed = 0;
	struct zxay_songblks *blk = NULL;
	struct zxay_song *song = NULL;
	
	int16_t extra_data = 0;
	int16_t extra_songblks = 0;
	
	fp = fopen(filename, "wb");
	if(fp == NULL) return false;

	squashed = zxay_data_dedupe(zxay);

	if(debug > 1) printf("Duplicate blocks: %d\n\n", squashed);

	/* Header
	 * Author
	 * Misc
	 */
	
	if(zxay->author) { /* if this is NULL the structure will already contain NULL offset */
		zxay_put_int16(sizeof(struct zxay_header) - PAUTHOR_OFFSET, zxay->header->PAuthor);
		extra_data = strlen(zxay->author) + 1; /* plus 1 for NULL */
	}
	
	if(zxay->misc) {
		zxay_put_int16(sizeof(struct zxay_header) - PMISC_OFFSET + extra_data, zxay->header->PMisc);
		extra_data += strlen(zxay->misc) + 1; /* plus 1 for NULL */
	}
	
	zxay_put_int16(sizeof(struct zxay_header) - PSONGSSTRUCTURE_OFFSET + extra_data, zxay->header->PSongsStructure);
	
	zxay_write(fp, (uint8_t *)zxay->header, sizeof(struct zxay_header), debug, ZXAY_S_HEADER);
	zxay_write(fp, (uint8_t *)zxay->author, strlen(zxay->author) + 1, debug, ZXAY_S_TEXT);
	zxay_write(fp, (uint8_t *)zxay->misc, strlen(zxay->misc) + 1, debug, ZXAY_S_TEXT);

	/* SongsStructure
	 * SongName }
	 * SongData }
	 * 
	 * SongPtrs }
	 * SongBlks }
	 * 
	 * DataBlks }
	 */
	 
	extra_data = 0;
	
	song = zxay->songsstruct;
	
	for(i = 0; i <= zxay->header->NumOfSongs; i++) {
		if(zxay->songname[i]) { /* if this is NULL the structure will already contain NULL offset */
			zxay_put_int16(sizeof(struct zxay_song) * (zxay->header->NumOfSongs - i) + sizeof(struct zxay_song) - PSONGNAME_OFFSET + extra_data, song->PSongName);
			extra_data += strlen(zxay->songname[i]) + 1; /* plus 1 for NULL */
		}

		zxay_put_int16(sizeof(struct zxay_song) * (zxay->header->NumOfSongs - i) + sizeof(struct zxay_song) - PSONGDATA_OFFSET + extra_data, song->PSongData);
		extra_data += sizeof(struct zxay_songdata);

		song++;
	}

	extra_data -= sizeof(struct zxay_songdata) * (zxay->header->NumOfSongs + 1);

	for(i = 0; i <= zxay->header->NumOfSongs; i++) {
		extra_data -= (strlen(zxay->songname[i]) + 1);

		zxay_put_int16(sizeof(struct zxay_songdata) * (zxay->header->NumOfSongs - i) + sizeof(struct zxay_songdata) - PPOINTS_OFFSET + extra_data, zxay->songdata[i]->PPoints);
		extra_data += sizeof(struct zxay_songptrs);

		zxay_put_int16(sizeof(struct zxay_songdata) * (zxay->header->NumOfSongs - i) + sizeof(struct zxay_songdata) - PADDRESSES_OFFSET + extra_data, zxay->songdata[i]->PAddresses);
		extra_data += sizeof(struct zxay_songblks) * (zxay->songblkcount[i] + 1);
		extra_songblks += sizeof(struct zxay_songblks) * (zxay->songblkcount[i] + 1);
	}

	extra_data = sizeof(struct zxay_songptrs) * zxay->header->NumOfSongs;
	
	for(i = 0; i <= zxay->header->NumOfSongs; i++) {
		blk = zxay->songblks[i];
		for(b = 0; b <= (zxay->songblkcount[i] + 1); b++) {
			if(!((blk->Address[0] == 0) && (blk->Address[1] == 0))) {
				//printf("es: %d b: %x size: %x ed: %x blkcount:%d do: %d\n" ,extra_songblks, b, sizeof(struct zxay_songblks), extra_data, zxay->songblkcount[i], zxay_data_offset(zxay, i, b));
				zxay_put_int16(extra_songblks - (b * sizeof(struct zxay_songblks)) - OFFSET_OFFSET + extra_data + zxay_data_offset(zxay, i, b), blk->Offset);
				//extra_data += zxay_read_int16(blk->Length);
			}
			blk++;
		}
		
		extra_songblks -= sizeof(struct zxay_songblks) * (zxay->songblkcount[i] + 1);
		extra_data -= sizeof(struct zxay_songptrs);
		//printf("es: %x \n" ,extra_songblks);
	}

	zxay_write(fp, (uint8_t *)zxay->songsstruct, sizeof(struct zxay_song) * (zxay->header->NumOfSongs + 1), debug, ZXAY_S_SONG);
	
	for(i = 0; i <= zxay->header->NumOfSongs; i++) {
		zxay_write(fp, (uint8_t *)zxay->songname[i], strlen(zxay->songname[i]) + 1, debug, ZXAY_S_TEXT);
		zxay_write(fp, (uint8_t *)zxay->songdata[i], sizeof(struct zxay_songdata), debug, ZXAY_S_SONGDATA);
	}
	
	for(i = 0; i <= zxay->header->NumOfSongs; i++) {
		zxay_write(fp, (uint8_t *)zxay->songptrs[i], sizeof(struct zxay_songptrs), debug, ZXAY_S_PTRS);
		zxay_write(fp, (uint8_t *)zxay->songblks[i], sizeof(struct zxay_songblks) * (zxay->songblkcount[i] + 1), debug, ZXAY_S_BLKS);
	}
		
	for(i = 0; i <= zxay->header->NumOfSongs; i++) {
		for(uint32_t n = 0; n < zxay->songblkcount[i]; n++) {
			zxay_write(fp, (uint8_t *)zxay->datablocks[i]->data[n], zxay->datablocks[i]->len[n], debug, ZXAY_S_DATA);
		}
	}
	
	fclose(fp);
	
	return true;
}

