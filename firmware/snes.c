/* Super NES Controller USB adapter */

#include <avr/eeprom.h>
#include <string.h>

#include "usbdrv.h"
#include "serial_pad.h"
#include "usb_descriptors.h"
#include "hid_pad8.h"
#include "hid_pad10.h"
#include "hid_keyboard.h"
#include "utils.h"


enum DeviceType
{
	PAD_8BUTTONS,
	PAD_10BUTTONS,
	PAD_KEYBOARD1,
	PAD_KEYBOARD2,
	PAD_NONE,
};

#define DEVICE_TYPES_NO PAD_NONE


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
static EEMEM uint8_t swapABEprom[DEVICE_TYPES_NO];
static EEMEM uint8_t swapXYEprom[DEVICE_TYPES_NO];
static EEMEM uint8_t swapTriggersEprom;


static void selectDeviceMode(enum DeviceType device);
static void configDevice();
static void sendPad8Report();
static void sendPad10Report();
static void sendKeyboard1Report();
static void sendKeyboard2Report();


int main(void)
{
	swapTriggers = eeprom_read_byte(&swapTriggersEprom) == 1;

	selectDeviceMode(eeprom_read_byte(&selectedModeEprom));
	padInit();


	while (1)
	{
		usbPoll();
		padReadData((uint8_t*) &padState, sizeof(padState));

		if (padState.select && padState.start)
		{
			configDevice();
			memset(&padState, 0, sizeof(padState));
		}

		if (swapAB)
			swap(&padState.a, &padState.b);

		if (swapXY)
			swap(&padState.x, &padState.y);

		if (usbInterruptIsReady())
		{
			switch (selectedMode)
			{
				default:
				case PAD_8BUTTONS:  sendPad8Report(); break;
				case PAD_10BUTTONS: sendPad10Report(); break;
				case PAD_KEYBOARD1: sendKeyboard1Report(); break;
				case PAD_KEYBOARD2: sendKeyboard2Report(); break;
			}
		}
	}
}


void sendPad8Report()
{
	static struct Pad8Report report;

	report.axisX = 1 - padState.left + padState.right;
	report.axisY = 1 - padState.up + padState.down;
	report.a = padState.a;
	report.b = padState.b;
	report.x = padState.x;
	report.y = padState.y;
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
	report.a = padState.a;
	report.b = padState.b;
	report.x = padState.x;
	report.y = padState.y;
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


void sendKeyboard1Report()
{
	static const struct PadState codes = {
		.x      = KEY_M,
		.y      = KEY_N,
		.l      = KEY_J,
		.r      = KEY_K,
		.select = KEY_ESC,
		.start  = KEY_ENTER,
		.up     = KEY_UP,
		.down   = KEY_DOWN,
		.left   = KEY_LEFT,
		.right  = KEY_RIGHT,
	};

	static const struct PadState modifiers = {
		.a      = KEY_MOD_LCTRL,
		.b      = KEY_MOD_LSHIFT,
	};

	static struct KeyboardReport report;

	keyboardMakeReport(&report,
			(uint8_t*) &codes,
			(uint8_t*) &modifiers,
			(uint8_t*) &padState,
			sizeof(padState));

	usbSetInterrupt((uchar*) &report, sizeof(report));
}


void sendKeyboard2Report()
{
	static const struct PadState codes = {
		.b      = KEY_UP,
		.x      = KEY_M,
		.y      = KEY_N,
		.l      = KEY_J,
		.r      = KEY_K,
		.select = KEY_ESC,
		.start  = KEY_ENTER,
		.up     = KEY_UP,
		.down   = KEY_DOWN,
		.left   = KEY_LEFT,
		.right  = KEY_RIGHT,
	};

	static const struct PadState modifiers = {
		.a      = KEY_MOD_LCTRL,
	};

	static struct KeyboardReport report;

	keyboardMakeReport(&report,
			(uint8_t*) &codes,
			(uint8_t*) &modifiers,
			(uint8_t*) &padState,
			sizeof(padState));

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
				usbConfig(USB_DEVICE_JOYSTICK,
						deviceName,
						&usbHidReportDescriptorPad8,
						sizeof(usbHidReportDescriptorPad8));
				break;

			case PAD_10BUTTONS:
				usbConfig(USB_DEVICE_JOYSTICK,
						deviceName,
						&usbHidReportDescriptorPad10,
						sizeof(usbHidReportDescriptorPad10));
				break;

			case PAD_KEYBOARD1:
			case PAD_KEYBOARD2:
				usbConfig(USB_DEVICE_KEYBOARD,
						deviceName,
						&usbHidReportDescriptorKeyboard,
						sizeof(usbHidReportDescriptorKeyboard));

				break;
		}

		selectedMode = mode;
		eeprom_write_byte(&selectedModeEprom, mode);

		swapAB = eeprom_read_byte(&swapABEprom[mode]) == 1;
		swapXY = eeprom_read_byte(&swapXYEprom[mode]) == 1;
	}
}


void configDevice()
{
	if (padState.a ^ padState.b)
	{
		swapAB = padState.b;
		eeprom_write_byte(&swapABEprom[selectedMode], swapAB);
	}

	if (padState.x ^ padState.y)
	{
		swapXY = padState.y;
		eeprom_write_byte(&swapXYEprom[selectedMode], swapXY);
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
	else if (padState.left)
		selectDeviceMode(PAD_KEYBOARD1);
	else if (padState.right)
		selectDeviceMode(PAD_KEYBOARD2);
}
