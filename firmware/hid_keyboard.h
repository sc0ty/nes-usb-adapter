#ifndef __HID_KEYBOARD_H__
#define __HID_KEYBOARD_H__

#include <stdint.h>

static const uint8_t usbHidReportDescriptorKeyboard[] = {
	0x05, 0x01,                         // Usage Page (Generic Desktop)
	0x09, 0x06,                         // Usage (Keyboard)
	0xA1, 0x01,                         // Collection (Application)
	0x05, 0x07,                         //     Usage Page (Key Codes)
	0x19, 0xe0,                         //     Usage Minimum (224)
	0x29, 0xe7,                         //     Usage Maximum (231)
	0x15, 0x00,                         //     Logical Minimum (0)
	0x25, 0x01,                         //     Logical Maximum (1)
	0x75, 0x01,                         //     Report Size (1)
	0x95, 0x08,                         //     Report Count (8)
	0x81, 0x02,                         //     Input (Data, Variable, Absolute)
	0x95, 0x01,                         //     Report Count (1)
	0x75, 0x08,                         //     Report Size (8)
	0x81, 0x01,                         //     Input (Constant) reserved byte(1)
	0x95, 0x05,                         //     Report Count (5)
	0x75, 0x01,                         //     Report Size (1)
	0x05, 0x08,                         //     Usage Page (LEDs)
	0x19, 0x01,                         //     Usage Minimum (1)
	0x29, 0x05,                         //     Usage Maximum (5)
	0x91, 0x02,                         //     Output (Data, Variable, Absolute), Led report
	0x95, 0x01,                         //     Report Count (1)
	0x75, 0x03,                         //     Report Size (3)
	0x91, 0x01,                         //     Output (Data, Variable, Absolute), Led report padding
	0x95, 0x06,                         //     Report Count (6)
	0x75, 0x08,                         //     Report Size (8)
	0x15, 0x00,                         //     Logical Minimum (0)
	0x25, 0xff,                         //     Logical Maximum (255)
	0x05, 0x07,                         //     Usage Page (Key codes)
	0x19, 0x00,                         //     Usage Minimum (0)
	0x29, 0xff,                         //     Usage Maximum (255)
	0x81, 0x00,                         //     Input (Data, Array) Key array(6 bytes)
	0xC0                                // End Collection (Application)
};

struct KeyboardReport
{
	uint8_t modifiers;
	uint8_t reserved;
	uint8_t keycodes[6];
};

enum KeyboardModifiers
{
	KEY_MOD_LCTRL  = 1 << 0,
	KEY_MOD_LSHIFT = 1 << 1,
	KEY_MOD_LALT   = 1 << 2,
	KEY_MOD_LGUI   = 1 << 3,
	KEY_MOD_RCTRL  = 1 << 4,
	KEY_MOD_RSHIFT = 1 << 5,
	KEY_MOD_RALT   = 1 << 6,
	KEY_MOD_RGUI   = 1 << 7,
};

enum KeyboardCodes
{
	KEY_A = 4,
	KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G, KEY_H, KEY_I, KEY_J, KEY_K, KEY_L,
	KEY_M, KEY_N, KEY_O, KEY_P, KEY_Q, KEY_R, KEY_S, KEY_T, KEY_U, KEY_V, KEY_W,
	KEY_X, KEY_Y, KEY_Z,

	KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9, KEY_0,

	KEY_ENTER,
	KEY_ESC,
	KEY_BACKSPACE,
	KEY_TAB,
	KEY_SPACE,

	KEY_RIGHT = 79,
	KEY_LEFT,
	KEY_DOWN,
	KEY_UP,
};


static inline void keyboardMakeReport(
		struct KeyboardReport *report,
		const uint8_t *codes,
		const uint8_t *modifiers,
		const uint8_t *mask,
		uint8_t len)
{
	uint8_t i;
	uint8_t reportNo = 0;

	report->modifiers = 0;
	report->reserved = 0;

	for (i = 0; i < len; i++)
	{
		if (mask[i])
		{
			if (codes[i] && reportNo < sizeof(report->keycodes))
				report->keycodes[reportNo++] = codes[i];

			if (modifiers[i])
				report->modifiers |= modifiers[i];
		}
	}

	while (reportNo < sizeof(report->keycodes))
		report->keycodes[reportNo++] = 0;
}

#endif
