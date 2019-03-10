#ifndef __HID_PAD10_H__
#define __HID_PAD10_H__

#include <stdint.h>

static uint8_t usbHidReportDescriptorPad10[] = {
	0x05, 0x01,     // USAGE_PAGE (Generic Desktop)
	0x09, 0x05,     // USAGE (Game Pad)
	0xa1, 0x01,     // COLLECTION (Application)
	0x09, 0x01,     //   USAGE (Pointer)
	0xa1, 0x00,     //   COLLECTION (Physical)
	0x09, 0x30,     //     USAGE (X)
	0x09, 0x31,     //     USAGE (Y)
	0x15, 0x00,     //     LOGICAL_MINIMUM (0)
	0x25, 0x02,     //     LOGICAL_MAXIMUM (2)
	0x75, 0x02,     //     REPORT_SIZE (2)
	0x95, 0x02,     //     REPORT_COUNT (2)
	0x81, 0x02,     //     INPUT (Data,Var,Abs)
	0xc0,           //   END_COLLECTION
	0x05, 0x09,     //   USAGE_PAGE (Button)
	0x19, 0x01,     //     USAGE_MINIMUM (Button 1)
	0x29, 0x0a,     //     USAGE_MAXIMUM (Button 10)
	0x15, 0x00,     //     LOGICAL_MINIMUM (0)
	0x25, 0x01,     //     LOGICAL_MAXIMUM (1)
	0x75, 0x01,     //     REPORT_SIZE (1)
	0x95, 0x0a,     //     REPORT_COUNT (10)
	0x81, 0x02,     //     INPUT (Data,Var,Abs)
	0x95, 0x01,     //     REPORT_COUNT (1)
	0x75, 0x02,     //     REPORT_SIZE (2)
	0x81, 0x03,     //     INPUT (Cnst,Var,Abs)
	0xc0            // END_COLLECTION
};

struct Pad10State
{
	uint16_t axisX : 2;
	uint16_t axisY : 2;
	uint16_t a : 1;
	uint16_t b : 1;
	uint16_t x : 1;
	uint16_t y : 1;
	uint16_t lShoulder : 1;
	uint16_t rShoulder : 1;
	uint16_t lTrigger : 1;
	uint16_t rTrigger : 1;
	uint16_t select : 1;
	uint16_t start : 1;
	uint16_t _padding : 2;
};

#endif
