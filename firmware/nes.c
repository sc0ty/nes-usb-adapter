#include <avr/eeprom.h>

#include "usbdrv.h"
#include "serial_pad.h"
#include "usb_descriptors.h"
#include "hid_pad4.h"
#include "hid_pad10.h"


enum DeviceType
{
	PAD_4BUTTONS,
	PAD_10BUTTONS,
	PAD_NONE,
};


enum DeviceType selectedMode = PAD_NONE;
uint8_t swapAB = 0;

static EEMEM uint8_t selectedModeEprom;
static EEMEM uint8_t swapABEprom;


static inline struct Pad4State readPadState();
static void selectDeviceMode(enum DeviceType device);
static inline void configDevice(struct Pad4State padState);


int main(void)
{
	struct Pad4State padState;
	struct Pad10State pad10State;

	swapAB = eeprom_read_byte(&swapABEprom) == 1;

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
				padState = pad4SwapAB(padState);

			switch (selectedMode)
			{
				default:
				case PAD_4BUTTONS:
					usbSetInterrupt((uchar*) &padState, sizeof(padState));
					break;

				case PAD_10BUTTONS:
					pad10State.axisX = padState.axisX;
					pad10State.axisY = padState.axisY;
					pad10State.a = padState.a;
					pad10State.b = padState.b;
					pad10State.lShoulder = 0;
					pad10State.rShoulder = 0;
					pad10State.lTrigger = 0;
					pad10State.rTrigger = 0;
					pad10State.select = padState.select;
					pad10State.start = padState.start;
					usbSetInterrupt((uchar*) &pad10State, sizeof(pad10State));
					break;
			}
		}
	}
}


struct Pad4State readPadState()
{
	struct Pad4State state;

	padLatchData();
	state.a = padReadBit();
	state.b = padReadBit();
	state.select = padReadBit();
	state.start = padReadBit();
	state.axisY = padReadAxis();
	state.axisX = padReadAxis();
	return state;
}


void selectDeviceMode(enum DeviceType mode)
{
	if (mode != selectedMode || selectedMode == PAD_NONE)
	{
		switch (mode)
		{
			default:
				mode = PAD_4BUTTONS;
				/* fallthrough */

			case PAD_4BUTTONS:
				usbConfig(&usbHidReportDescriptorPad4,
					sizeof(usbHidReportDescriptorPad4));
				break;

			case PAD_10BUTTONS:
				usbConfig(&usbHidReportDescriptorPad10,
					sizeof(usbHidReportDescriptorPad10));
				break;
		}

		selectedMode = mode;
		eeprom_write_byte(&selectedModeEprom, mode);
	}
}


void configDevice(struct Pad4State padState)
{
	if (padState.a ^ padState.b)
	{
		swapAB = padState.b;
		eeprom_write_byte(&swapABEprom, swapAB);
	}

	if (padState.axisY == 0)	// up
	{
		selectDeviceMode(PAD_4BUTTONS);
	}

	if (padState.axisY == 2)	// down
	{
		selectDeviceMode(PAD_10BUTTONS);
	}
}
