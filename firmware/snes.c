/* Super NES Controller USB adapter */

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


struct PadState
{
	uint8_t b;
	uint8_t y;
	uint8_t select;
	uint8_t start;
	uint8_t up;
	uint8_t down;
	uint8_t left;
	uint8_t right;
	uint8_t a;
	uint8_t x;
	uint8_t l;
	uint8_t r;
};


struct PadState padState;
enum DeviceType selectedMode = PAD_NONE;
uint8_t swapAB = 0;
uint8_t swapXY = 0;
uint8_t swapTriggers = 0;

static EEMEM uint8_t selectedModeEprom;
static EEMEM uint8_t swapABEprom;
static EEMEM uint8_t swapXYEprom;
static EEMEM uint8_t swapTriggersEprom;


static void selectDeviceMode(enum DeviceType device);
static inline void configDevice();
static void sendPad8Report();
static void sendPad10Report();


int main(void)
{
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
			padReadData((uint8_t*) &padState, sizeof(padState));

			if (padState.select && padState.start)
				configDevice(padState);

			switch (selectedMode)
			{
				default:
				case PAD_8BUTTONS:
					sendPad8Report();
					break;

				case PAD_10BUTTONS:
					sendPad10Report();
					break;
			}
		}
	}
}


void sendPad8Report()
{
	static struct Pad8Report report;

	report.axisX = 1 - padState.left + padState.right;
	report.axisY = 1 - padState.up + padState.down;
	report.a = !swapAB ? padState.a : padState.b;
	report.b = !swapAB ? padState.b : padState.a;
	report.x = !swapXY ? padState.x : padState.y;
	report.y = !swapXY ? padState.y : padState.x;
	report.l = padState.l;
	report.r = padState.r;
	report.select = padState.select;
	report.start = padState.start;

	usbSetInterrupt((uchar*) &report, sizeof(report));
}


void sendPad10Report()
{
	static struct Pad10Report report;

	report.axisX = 1 - padState.left + padState.right;
	report.axisY = 1 - padState.up + padState.down;
	report.a = !swapAB ? padState.a : padState.b;
	report.b = !swapAB ? padState.b : padState.a;
	report.x = !swapXY ? padState.x : padState.y;
	report.y = !swapXY ? padState.y : padState.x;
	report.select = padState.select;
	report.start = padState.start;

	if (swapTriggers)
	{
		report.lShoulder = 0;
		report.rShoulder = 0;
		report.lTrigger = padState.l;
		report.rTrigger = padState.r;
	}
	else
	{
		report.lShoulder = padState.l;
		report.rShoulder = padState.r;
		report.lTrigger = 0;
		report.rTrigger = 0;
	}

	usbSetInterrupt((uchar*) &report, sizeof(report));
}


void selectDeviceMode(enum DeviceType mode)
{
	static const char deviceName[] = "SNES Controller";

	if (mode != selectedMode || selectedMode == PAD_NONE)
	{
		switch (mode)
		{
			default:
				mode = PAD_8BUTTONS;
				/* fallthrough */

			case PAD_8BUTTONS:
				usbConfig(usbJoystickDeviceId,
						deviceName,
						&usbHidReportDescriptorPad8,
						sizeof(usbHidReportDescriptorPad8));
				break;

			case PAD_10BUTTONS:
				usbConfig(usbJoystickDeviceId,
						deviceName,
						&usbHidReportDescriptorPad10,
						sizeof(usbHidReportDescriptorPad10));
				break;
		}

		selectedMode = mode;
		eeprom_write_byte(&selectedModeEprom, mode);
	}
}


void configDevice()
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

	if (padState.up)
		selectDeviceMode(PAD_8BUTTONS);
	else if (padState.down)
		selectDeviceMode(PAD_10BUTTONS);
}
