#include <avr/eeprom.h>

#include "usbdrv.h"
#include "serial_pad.h"
#include "usb_descriptors.h"
#include "hid_pad8.h"
#include "hid_pad10.h"


enum DeviceType
{
	PAD_8BUTTONS,
	PAD_10BUTTONS,
	PAD_NONE,
};


enum DeviceType selectedMode = PAD_NONE;
uint8_t swapAB = 0;
uint8_t swapXY = 0;
uint8_t swapTriggers = 0;

static EEMEM uint8_t selectedModeEprom;
static EEMEM uint8_t swapABEprom;
static EEMEM uint8_t swapXYEprom;
static EEMEM uint8_t swapTriggersEprom;


static inline struct Pad8State readPadState();
static void selectDeviceMode(enum DeviceType device);
static inline void configDevice(struct Pad8State padState);


int main(void)
{
	struct Pad8State padState;
	struct Pad10State pad10State;

	swapAB = eeprom_read_byte(&swapABEprom) == 1;
	swapXY = eeprom_read_byte(&swapXYEprom) == 1;
	swapTriggers = eeprom_read_byte(&swapTriggersEprom) == 1;

	selectDeviceMode(eeprom_read_byte(&selectedModeEprom));
	padInit();


	while (1)
	{
		usbPoll();

		if (usbInterruptIsReady())
		{
			padState = readPadState();

			if (padState.select && padState.start)
				configDevice(padState);

			if (swapAB)
				padState = pad8SwapAB(padState);

			if (swapXY)
				padState = pad8SwapXY(padState);

			switch (selectedMode)
			{
				default:
				case PAD_8BUTTONS:
					usbSetInterrupt((uchar*) &padState, sizeof(padState));
					break;

				case PAD_10BUTTONS:
					pad10State.axisX = padState.axisX;
					pad10State.axisY = padState.axisY;
					pad10State.a = padState.a;
					pad10State.b = padState.b;
					pad10State.x = padState.x;
					pad10State.y = padState.y;

					if (swapTriggers)
					{
						pad10State.lShoulder = 0;
						pad10State.rShoulder = 0;
						pad10State.lTrigger = padState.l;
						pad10State.rTrigger = padState.r;
					}
					else
					{
						pad10State.lShoulder = padState.l;
						pad10State.rShoulder = padState.r;
						pad10State.lTrigger = 0;
						pad10State.rTrigger = 0;
					}

					pad10State.select = padState.select;
					pad10State.start = padState.start;
					usbSetInterrupt((uchar*) &pad10State, sizeof(pad10State));
					break;
			}
		}
	}
}


struct Pad8State readPadState()
{
	struct Pad8State state;

	padLatchData();
	state.b = padReadBit();
	state.y = padReadBit();
	state.select = padReadBit();
	state.start = padReadBit();
	state.axisY = padReadAxis();
	state.axisX = padReadAxis();
	state.a = padReadBit();
	state.x = padReadBit();
	state.l = padReadBit();
	state.r = padReadBit();
	return state;
}


void selectDeviceMode(enum DeviceType mode)
{
	if (mode != selectedMode || selectedMode == PAD_NONE)
	{
		switch (mode)
		{
			default:
				mode = PAD_8BUTTONS;
				/* fallthrough */

			case PAD_8BUTTONS:
				usbConfig(usbJoystickDeviceId,
						"SNES Controller",
						&usbHidReportDescriptorPad8,
						sizeof(usbHidReportDescriptorPad8));
				break;

			case PAD_10BUTTONS:
				usbConfig(usbJoystickDeviceId,
						"SNES Controller",
						&usbHidReportDescriptorPad10,
					sizeof(usbHidReportDescriptorPad10));
				break;
		}

		selectedMode = mode;
		eeprom_write_byte(&selectedModeEprom, mode);
	}
}


void configDevice(struct Pad8State padState)
{
	if (padState.a ^ padState.b)
	{
		swapAB = padState.b;
		eeprom_write_byte(&swapABEprom, swapAB);
	}

	if (padState.x ^ padState.y)
	{
		swapAB = padState.y;
		eeprom_write_byte(&swapABEprom, swapXY);
	}

	if (padState.l ^ padState.r)
	{
		swapTriggers = padState.r;
		eeprom_write_byte(&swapTriggersEprom, swapTriggers);
	}

	if (padState.axisY == 0)	// up
	{
		selectDeviceMode(PAD_8BUTTONS);
	}

	if (padState.axisY == 2)	// down
	{
		selectDeviceMode(PAD_10BUTTONS);
	}
}
