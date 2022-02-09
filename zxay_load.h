/* AYE ZXAYEMUL metadata editor
 * by Chris Young 2021, 2022
 * https://www.unsatisfactorysoftware.co.uk
 */
 
/* ZXAY read routines */
 
#ifndef ZXAY_LOAD_H
#define ZXAY_LOAD_H 1

void *zxay_load(char *filename);
void zxay_free(void *zxayemul);

#endif
