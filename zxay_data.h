/* AYE ZXAYEMUL metadata editor
 * by Chris Young 2021, 2022
 * https://www.unsatisfactorysoftware.co.uk
 */
 
/* ZXAY data routines */

#ifndef ZXAY_DATA_H
#define ZXAY_DATA_H 1

#include <stdint.h>

/* Find a block offset */
uint32_t zxay_data_offset(void *zxayemul, int song, int block);

/* Dedupe data blocks */
uint32_t zxay_data_dedupe(void *zxayemul);
#endif
