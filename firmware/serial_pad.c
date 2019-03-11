/* Serial NES Controller interface */

#include "serial_pad.h"

#include <avr/io.h>
#include <util/delay.h>


#define NES_DDR DDRB
#define NES_PORT PORTB
#define NES_PIN PINB

#define NES_CLK   0
#define NES_DATA  2
#define NES_LATCH 1


void padInit()
{
	NES_DDR |= _BV(NES_CLK) | _BV(NES_LATCH);
	NES_DDR &= ~_BV(NES_DATA);
	NES_PORT |= _BV(NES_CLK) | _BV(NES_LATCH);
}

void padReadData(uint8_t *data, uint8_t len)
{
	uint8_t i;
	uint8_t ones;

	// latch data
	NES_PORT |= _BV(NES_LATCH);
	_delay_us(1);
	NES_PORT &= ~_BV(NES_LATCH);
	_delay_us(1);

	data[0] = !(NES_PIN & _BV(NES_DATA));
	ones = data[0];

	for (i = 1; i < len; i++)
	{
		// shift data to next bit
		NES_PORT |= _BV(NES_CLK);
		_delay_us(1);
		NES_PORT &= ~_BV(NES_CLK);
		_delay_us(1);

		data[i] = !(NES_PIN & _BV(NES_DATA));
		ones += data[i];
	}

	if (ones == len)
	{
		// all ones - pad disconnected, clearing state
		for (i = 0; i < len; i++)
			data[i] = 0;
	}
}

