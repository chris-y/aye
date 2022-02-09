/* AYE ZXAYEMUL metadata editor
 * by Chris Young 2021
 * https://www.unsatisfactorysoftware.co.uk
 */
 
/* ZXAY debug routines */
 
#ifndef ZXAY_DUMP_H
#define ZXAY_DUMP_H 1

#include <inttypes.h>

enum {
        ZXAY_S_NONE = 0,
	ZXAY_S_UNKNOWN,
        ZXAY_S_HEADER,
	ZXAY_S_SONG,
	ZXAY_S_SONGDATA,
	ZXAY_S_PTRS,
	ZXAY_S_BLKS,
	ZXAY_S_DATA,
	ZXAY_S_TEXT
};

void zxay_dump(void *zxayemul, int level);
void zxay_dump_struct(uint8_t *structure, size_t size, int type);

#endif
