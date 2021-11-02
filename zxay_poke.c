/* AYE ZXAYEMUL metadata editor
 * by Chris Young 2021
 * https://www.unsatisfactorysoftware.co.uk
 */
 
/* ZXAY peek routines */

#include <stdlib.h>
#include <string.h>

#include "zxay_file.h"
#include "zxay_poke.h"

void zxay_poke(void *zxayemul, int type, char *newvalue)
{
	struct zxay_file *zxay = (struct zxay_file *)zxayemul;
	
	switch(type) {
		case ZXAY_AUTHOR:
			if(zxay->author) free(zxay->author);
			zxay->author = strdup(newvalue);
		break;
		case ZXAY_MISC:
			if(zxay->misc) free(zxay->misc);
			zxay->misc = strdup(newvalue);
		break;
		default:
		break;
	}
}
