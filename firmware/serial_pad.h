#ifndef __SERIAL_PAD_H__
#define __SERIAL_PAD_H__

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

#define NES_DDR DDRB
#define NES_PORT PORTB
#define NES_PIN PINB

#define NES_CLK   0
#define NES_DATA  2
#define NES_LATCH 1


static inline void padInit()
{
	NES_DDR |= _BV(NES_CLK) | _BV(NES_LATCH);
	NES_DDR &= ~_BV(NES_DATA);
	NES_PORT |= _BV(NES_CLK) | _BV(NES_LATCH);
}

static inline void padLatchData()
{
	NES_PORT |= _BV(NES_LATCH);
	_delay_us(1);
	NES_PORT &= ~_BV(NES_LATCH);
	_delay_us(1);
}

static inline uint8_t padReadBit()
{
	uint8_t val = !(NES_PIN & _BV(NES_DATA));

	NES_PORT |= _BV(NES_CLK);
	_delay_us(1);
	NES_PORT &= ~_BV(NES_CLK);
	_delay_us(1);

	return val;
}

static inline uint8_t padReadAxis()
{
	uint8_t val = 1;
	val -= padReadBit();
	val += padReadBit();
	return val;
}

#endif
