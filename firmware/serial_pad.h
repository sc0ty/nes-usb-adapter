/* Serial NES Controller interface */

#ifndef __SERIAL_PAD_H__
#define __SERIAL_PAD_H__

#include <stdint.h>


void padInit();
void padReadData(uint8_t *data, uint8_t len);

#endif
