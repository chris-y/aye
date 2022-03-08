/* AYE ZXAYEMUL metadata editor
 * by Chris Young 2021, 2022
 * https://www.unsatisfactorysoftware.co.uk
 */

/* ZXAY merge (join) routines */

#ifndef ZXAY_MERGE_H
#define ZXAY_MERGE_H 1

/* Merge two structures. NB: The data will be owned by the new combined structure */
void *zxay_merge(void *zxayemul, void *zxayemulj);
#endif
