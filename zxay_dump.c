/* AYE ZXAYEMUL metadata editor
 * by Chris Young 2021
 * https://www.unsatisfactorysoftware.co.uk
 */
 
/* ZXAY debug routines */

#include <stdio.h>
#include <stdint.h>

#include "zxay_dump.h"
#include "zxay_file.h"

static uint32_t offset = 0;

static void dump(uint8_t *structure, size_t size)
{
	int i = 0;

	printf("[%04x] ", offset);

	for(i = 0; i < size; i++) {
		printf("%02x ", structure[i]);
	}
	
	printf("\n");
	offset += size;
}

void zxay_dump(void *zxayemul, int level)
{
	struct zxay_file *zxay = (struct zxay_file *)zxayemul;
	int i = 0;
	int b = 0;
	char *blk = NULL;
	
	printf("Header: ");
	dump((char *)zxay->header, sizeof(struct zxay_header));
	
	for(i = 0; i <= zxay->header->NumOfSongs; i++) {
		printf("Track %d:\n", i);
		printf("  Song struct: ");
		dump((char *)(zxay->songsstruct + i), sizeof(struct zxay_song));
		printf("  Song data: ");
		dump((char *)zxay->songdata[i], sizeof(struct zxay_songdata));
		printf("  Song ptrs: ");
		dump((char *)zxay->songptrs[i], sizeof(struct zxay_songptrs));
		printf("  Song blks:\n");
		blk = (char *)zxay->songblks[i];
		for(b = 0; b < (zxay->songblkcount[i]); b++) {
			printf("    Block %d: ", b);
			dump(blk, sizeof(struct zxay_songblks));
			blk += sizeof(struct zxay_songblks);
		}
		if(level > 1) {
			printf("    All block data: \n", b);
			dump((char *)zxay->datablks[i], zxay->datablk_size[i]);
		} else {
			offset += zxay->datablk_size[i];
		}
	}
}
