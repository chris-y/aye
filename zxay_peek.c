/* AYE ZXAYEMUL metadata editor
 * by Chris Young 2021
 * https://www.unsatisfactorysoftware.co.uk
 */
 
/* ZXAY peek routines */

#include <stddef.h>

#include "zxay_file.h"
#include "zxay_peek.h"

/* Peek at a song, song is 0 - ZXAY_SONGCOUNT */
char *zxay_peek_song(void *zxayemul, int song, int type)
{
	struct zxay_file *zxay = (struct zxay_file *)zxayemul;

	if(song > zxay->header->NumOfSongs) return NULL;
	
	switch(type) {
		case ZXAY_SONG_NAME:
			return zxay->songname[song];
		break;
		default:
			return NULL;
		break;
	}
	
	return NULL;
}

char *zxay_peek(void *zxayemul, int type)
{
	struct zxay_file *zxay = (struct zxay_file *)zxayemul;
	
	switch(type) {
		case ZXAY_AUTHOR:
			return zxay->author;
		break;
		case ZXAY_MISC:
			return zxay->misc;
		break;
		case ZXAY_SONGCOUNT:
			return &zxay->header->NumOfSongs;
		default:
			return NULL;
		break;
	}
	
	return NULL;
}
