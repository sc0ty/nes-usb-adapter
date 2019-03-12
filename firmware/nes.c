/* NES Controller USB adapter */

#include <avr/eeprom.h>
#include <string.h>

#include "usbdrv.h"
#include "serial_pad.h"
#include "usb_descriptors.h"
#include "hid_pad4.h"
#include "hid_pad10.h"
#include "hid_keyboard.h"


enum DeviceType
{
	PAD_4BUTTONS,
	PAD_10BUTTONS,
	PAD_KEYBOARD,
	PAD_NONE,
};


struct PadState
{
	uint8_t a;
	uint8_t b;
	uint8_t select;
	uint8_t start;
	uint8_t up;
	uint8_t down;
	uint8_t left;
	uint8_t right;
};


struct PadState padState;
enum DeviceType selectedMode = PAD_NONE;
uint8_t swapAB = 0;

static EEMEM uint8_t selectedModeEprom;
static EEMEM uint8_t swapABEprom;


static void selectDeviceMode(enum DeviceType device);
static void configDevice();
static void sendPad4Report();
static void sendPad10Report();
static void sendKeyboardReport();


int main(void)
{
	swapAB = eeprom_read_byte(&swapABEprom) == 1;

	selectDeviceMode(eeprom_read_byte(&selectedModeEprom));
	padInit();


	while (1)
	{
		usbPoll();
		padReadData((uint8_t*) &padState, sizeof(padState));

		if (usbInterruptIsReady())
		{
			switch (selectedMode)
			{
				default:
				case PAD_4BUTTONS:
					sendPad4Report();
					break;

				case PAD_10BUTTONS:
					sendPad10Report();
					break;

				case PAD_KEYBOARD:
					sendKeyboardReport();
					break;
			}
		}

		if (padState.select && padState.start)
			configDevice();
	}
}


void sendPad4Report()
{
	static struct Pad4Report report;

	report.axisX = 1 - padState.left + padState.right;
	report.axisY = 1 - padState.up + padState.down;
	report.a = !swapAB ? padState.a : padState.b;
	report.b = !swapAB ? padState.b : padState.a;
	report.select = padState.select;
	report.start = padState.start;

	usbSetInterrupt((uchar*) &report, sizeof(report));
}


void sendPad10Report()
{
	static struct Pad10Report report;

	report.axisX = 1 - padState.left + padState.right;
	report.axisY = 1 - padState.up + padState.down;
	report.a = !swapAB ? padState.b : padState.a;
	report.b = !swapAB ? padState.a : padState.b;
	report.lShoulder = 0;
	report.rShoulder = 0;
	report.lTrigger = 0;
	report.rTrigger = 0;
	report.x = 0;
	report.y = 0;
	report.select = padState.select;
	report.start = padState.start;

	usbSetInterrupt((uchar*) &report, sizeof(report));
}


void sendKeyboardReport()
{
	static const struct PadState codes = {
		.select = KEY_ESC,
		.start  = KEY_ENTER,
		.up     = KEY_UP,
		.down   = KEY_DOWN,
		.left   = KEY_LEFT,
		.right  = KEY_RIGHT
	};

	static struct KeyboardReport report;

	keyboardSetKeyCodes(&report,
			(uint8_t*) &codes,
			(uint8_t*) &padState,
			sizeof(padState));

	report.lCtrl  = !swapAB ? padState.a : padState.b;
	report.lShift = !swapAB ? padState.b : padState.a;

	usbSetInterrupt((uchar*) &report, sizeof(report));
}


void selectDeviceMode(enum DeviceType mode)
{
	static const char deviceName[] = "NES Controller";

	if (mode != selectedMode || selectedMode == PAD_NONE)
	{
		switch (mode)
		{
			default:
				mode = PAD_4BUTTONS;
				/* fallthrough */

			case PAD_4BUTTONS:
				usbConfig(USB_DEVICE_JOYSTICK,
						deviceName,
						&usbHidReportDescriptorPad4,
						sizeof(usbHidReportDescriptorPad4));
				break;

			case PAD_10BUTTONS:
				usbConfig(USB_DEVICE_JOYSTICK,
						deviceName,
						&usbHidReportDescriptorPad10,
						sizeof(usbHidReportDescriptorPad10));
				break;

			case PAD_KEYBOARD:
				usbConfig(USB_DEVICE_KEYBOARD,
						deviceName,
						&usbHidReportDescriptorKeyboard,
						sizeof(usbHidReportDescriptorKeyboard));
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

	if (padState.up)
		selectDeviceMode(PAD_4BUTTONS);
	else if (padState.down)
		selectDeviceMode(PAD_10BUTTONS);
	else if (padState.left)
		selectDeviceMode(PAD_KEYBOARD);
}
