/* AYE ZXAYEMUL metadata editor
 * by Chris Young 2021
 * https://www.unsatisfactorysoftware.co.uk
 */
 
/* ZXAY debug routines */

#include <stdio.h>
#include <stdint.h>

#include "zxay_dump.h"
#include "zxay_file.h"
#include "zxay_misc.h"

static uint32_t offset = 0;

static void dump(uint8_t *structure, size_t size, int type)
{
	int i = 0;
	int j = 0;

	if(type > 0) printf("[%04x] ", offset);

	for(i = 0; i < size; i++) {
		printf("%02x ", structure[i]);
		if(((type == ZXAY_S_HEADER) && ((i == (PAUTHOR_OFFSET + 1)) || (i == (PMISC_OFFSET + 1)) || (i == (PSONGSSTRUCTURE_OFFSET + 1)))) ||
			((type == ZXAY_S_SONG) && ((i == (PSONGNAME_OFFSET + 1)) || (i == (PSONGDATA_OFFSET + 1)))) ||
                        ((type == ZXAY_S_SONGDATA) && ((i == (PPOINTS_OFFSET + 1)) || (i == (PADDRESSES_OFFSET + 1)))) ||
                        ((type == ZXAY_S_BLKS) && (((i - (sizeof(struct zxay_songblks) * j)) == (OFFSET_OFFSET + 1)))) ) {
			printf("{%04x} ", zxay_read_int16(structure + i - 1) + (offset + i - 1));
			if(type == ZXAY_S_BLKS) j++;
		}
	}

	if(type == ZXAY_S_TEXT) printf("\n       {%s}", structure);

	printf("\n");
	if(type > 0) offset += size;
}

void zxay_dump_struct(uint8_t *structure, size_t size, int type)
{
	if(type == ZXAY_S_NONE) type = ZXAY_S_UNKNOWN;
	dump(structure, size, type);
}

void zxay_dump(void *zxayemul, int level)
{
	struct zxay_file *zxay = (struct zxay_file *)zxayemul;
	int i = 0;
	int b = 0;
	char *blk = NULL;
	
	printf("Header: ");
	dump((char *)zxay->header, sizeof(struct zxay_header), 0);
	
	for(i = 0; i <= zxay->header->NumOfSongs; i++) {
		printf("Track %d:\n", i);
		printf("  Song struct: ");
		dump((char *)(zxay->songsstruct + i), sizeof(struct zxay_song), 0);
		printf("  Song data: ");
		dump((char *)zxay->songdata[i], sizeof(struct zxay_songdata), 0);
		printf("  Song ptrs: ");
		dump((char *)zxay->songptrs[i], sizeof(struct zxay_songptrs), 0);
		printf("  Song blks:\n");
		blk = (char *)zxay->songblks[i];
		for(b = 0; b < (zxay->songblkcount[i]); b++) {
			printf("    Block %d: ", b);
			dump(blk, sizeof(struct zxay_songblks), 0);
			blk += sizeof(struct zxay_songblks);
		}
		if(level > 2) {
			printf("    All block data: \n", b);
			dump((char *)zxay->datablks[i], zxay->datablk_size[i], 0);
		}
	}
}
