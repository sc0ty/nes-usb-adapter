/* Utility functions */

#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdint.h>


static inline void swap(uint8_t *x, uint8_t *y)
{
	uint8_t t = *x;
	*x = *y;
	*y = t;
}

#endif
