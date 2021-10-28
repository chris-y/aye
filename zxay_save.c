/* AYE ZXAYEMUL metadata editor
 * by Chris Young 2021
 * https://www.unsatisfactorysoftware.co.uk
 */
 
/* ZXAY save routines */

#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "zxay_file.h"
#include "zxay_save.h"
#include "zxay_misc.h"

static void zxay_write(char *structure, size_t size)
{
	int i = 0;
	for(i = 0; i < size; i++) {
		printf("%02x ", structure[i]);
	}
	printf("\n");
}

static void zxay_put_int16(int16_t val, char *dest)
{
	dest[0] = (char)(val >> 8);
	dest[1] = (char)(val);
}

bool zxay_save(void *zxayemul, char *filename)
{
	struct zxay_file *zxay = (struct zxay_file *)zxayemul;
	int i = 0;
	int b = 0;
	struct zxay_songblks *blk = NULL;
	struct zxay_song *song = NULL;
	
	int16_t extra_data = 0;
	int16_t extra_songblks = 0;
	
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
	
	zxay_write((char *)zxay->header, sizeof(struct zxay_header));
	zxay_write((char *)zxay->author, strlen(zxay->author) + 1);
	zxay_write((char *)zxay->misc, strlen(zxay->misc) + 1);

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

	extra_data = 0;

	for(i = 0; i <= zxay->header->NumOfSongs; i++) {
		zxay_put_int16(sizeof(struct zxay_songdata) * (zxay->header->NumOfSongs - i) + sizeof(struct zxay_songdata) - PPOINTS_OFFSET + extra_data, zxay->songdata[i]->PPoints);
		extra_data += sizeof(struct zxay_songptrs);

		zxay_put_int16(sizeof(struct zxay_songdata) * (zxay->header->NumOfSongs - i) + sizeof(struct zxay_songdata) - PADDRESSES_OFFSET + extra_data, zxay->songdata[i]->PAddresses);
		extra_data += sizeof(struct zxay_songblks) * (zxay->songblkcount[i] + 1);
		extra_songblks += sizeof(struct zxay_songblks) * (zxay->songblkcount[i] + 1);
	}

	extra_data = 0;
	
	for(i = 0; i <= zxay->header->NumOfSongs; i++) {
		blk = zxay->songblks[i];
		for(b = 0; b <= zxay->songblkcount[i]; b++) {
			if((blk->Address[0] != 0) && (blk->Address[1] != 0))
				zxay_put_int16(extra_songblks - (b * sizeof(struct zxay_songblks)) - OFFSET_OFFSET + extra_data, blk->Offset);
			extra_data += zxay_read_int16(blk->Length);
			blk++;
		}
		
		extra_songblks -= sizeof(struct zxay_songblks) * (zxay->songblkcount[i] + 1);
	}

	zxay_write((char *)zxay->songsstruct, sizeof(struct zxay_song) * (zxay->header->NumOfSongs + 1));	
	
	for(i = 0; i <= zxay->header->NumOfSongs; i++) {
		zxay_write((char *)zxay->songname[i], strlen(zxay->songname[i]) + 1);
		zxay_write((char *)zxay->songdata[i], sizeof(struct zxay_songdata));
	}
	
	for(i = 0; i <= zxay->header->NumOfSongs; i++) {
		zxay_write((char *)zxay->songptrs[i], sizeof(struct zxay_songptrs));
		zxay_write((char *)zxay->songblks[i], sizeof(struct zxay_songblks) * (zxay->songblkcount[i] + 1));
	}
		
	for(i = 0; i <= zxay->header->NumOfSongs; i++) {
		zxay_write((char *)zxay->datablks[i], zxay->datablk_size[i]);
	}	
		
}

