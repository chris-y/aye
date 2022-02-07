/* AYE ZXAYEMUL metadata editor
 * by Chris Young 2021
 * https://www.unsatisfactorysoftware.co.uk
 */

/* zxay misc routines, for internal use */

#include <stdint.h>

#include "zxay_misc.h"

int16_t zxay_read_int16(uint8_t *val)
{
	int16_t i = 0;
	i = (val[0] << 8) | val[1];
	return i;
}
