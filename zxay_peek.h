/* AYE ZXAYEMUL metadata editor
 * by Chris Young 2021, 2022
 * https://www.unsatisfactorysoftware.co.uk
 */
 
/* ZXAY peek routines */

#ifndef ZXAY_PEEK_H
#define ZXAY_PEEK_H 1

#include "zxay_consts.h"

/* Peek at a file */
char *zxay_peek(void *zxayemul, int type);

/* Peek at a song, song is 0 - ZXAY_SONGCOUNT */
char *zxay_peek_song(void *zxayemul, int song, int type);

#endif
