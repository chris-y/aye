/* AYE ZXAYEMUL metadata editor
 * by Chris Young 2021
 * https://www.unsatisfactorysoftware.co.uk
 */
 
/* ZXAY poke routines */

#ifndef ZXAY_POKE_H
#define ZXAY_POKE_H 1

#include "zxay_consts.h"

/* Poke a file */
void zxay_poke(void *zxayemul, int type, char *newvalue);

#endif
