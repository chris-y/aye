/* AYE ZXAYEMUL metadata editor
 * by Chris Young 2021
 * https://www.unsatisfactorysoftware.co.uk
 */
 
/* ZXAY debug routines */

#include <stdio.h>
#include <stdint.h>

#include "zxay_dump.h"
#include "zxay_file.h"

static void dump(char *structure, size_t size)
{
	int i = 0;
	for(i = 0; i < size; i++) {
		printf("%02x ", structure[i]);
	}
	
	printf("\n");
}

void zxay_dump(void *zxayemul)
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
		dump((char *)(zxay->songsstruct + (sizeof(struct zxay_song) * i)), sizeof(struct zxay_song));
		printf("  Song data: ");
		dump((char *)zxay->songdata[i], sizeof(struct zxay_songdata));
		printf("  Song ptrs: ");
		dump((char *)zxay->songptrs[i], sizeof(struct zxay_songptrs));
		printf("  Song blks:\n");
		blk = (char *)zxay->songblks[i];
		for(b = 0; b < zxay->songblkcount[i]; b++) {
			printf("    Block %d: ", b);
			dump(blk, sizeof(struct zxay_songblks));
			blk += sizeof(struct zxay_songblks);
		}
	}
}