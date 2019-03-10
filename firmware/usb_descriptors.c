#include "usb_descriptors.h"
#include "usbdrv.h"
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stddef.h>


struct UsbDescriptors usbDescriptors = {
	.configuration = {
		.bLength                = sizeof(struct UsbConfigurationDescriptor),
		.bDescriptorType        = USBDESCR_CONFIG,
		.wTotalLength           = sizeof(struct UsbDescriptors),
		.bNumInterfaces         = 1,
		.bConfigurationValue    = 1,
		.iConfiguration         = 0,
		.bmAttributes           = 0x80,
		.bMaxPower              = USB_CFG_MAX_BUS_POWER / 2,
	},

	.interface = {
		.bLength                = sizeof(struct UsbInterfaceDescriptor),
		.bDescriptorType        = USBDESCR_INTERFACE,
		.bInterfaceNumber       = 0,
		.bAlternateSetting      = 0,
		.bNumEndpoints          = 1,
		.bInterfaceClass        = USB_CFG_INTERFACE_CLASS,
		.bInterfaceSubClass     = USB_CFG_INTERFACE_SUBCLASS,
		.bInterfaceProtocol     = USB_CFG_INTERFACE_PROTOCOL,
		.iInterface             = 0,
	},

	.hid = {
		.bLength                = sizeof(struct UsbHidDescriptor),
		.bDescriptorType        = USBDESCR_HID,
		.bcdHID                 = 0x0101,
		.bCountryCode           = 0x00,
		.bNumDescriptors        = 1,
		.bDescriptorType_HID    = 0x22, // report
		.wDescriptorLength      = 0,
	},

	.endpoint =
	{
		.bLength                = sizeof(struct UsbEndpointDescriptor),
		.bDescriptorType        = USBDESCR_ENDPOINT,
		.bEndpointAddress       = 0x81,
		.bmAttributes           = 0x03, // Interrupt-Transfer
		.wMaxPacketSize         = 8,
		.bInterval              = USB_CFG_INTR_POLL_INTERVAL,
	},
};


void *usbHidReportDescriptor = NULL;


void usbConfig(void *hidDescriptor, uint16_t hidDescriptorLen)
{
	cli();
	usbDeviceDisconnect();
	_delay_ms(500);

	usbHidReportDescriptor = hidDescriptor;
	usbDescriptors.hid.wDescriptorLength = hidDescriptorLen;

	usbDeviceConnect();
	usbInit();
	sei();
}

usbMsgLen_t usbFunctionSetup(uchar data[8])
{
	(void) data;
	return 0;
}

usbMsgLen_t usbFunctionDescriptor(struct usbRequest *rq)
{
	if (rq->bRequest == USBRQ_GET_DESCRIPTOR)
	{
		switch (rq->wValue.bytes[1])
		{
			case USBDESCR_CONFIG:
				usbMsgPtr = (usbMsgPtr_t) &usbDescriptors;
				return usbDescriptors.configuration.wTotalLength;

			case USBDESCR_HID:
				usbMsgPtr = (usbMsgPtr_t) &usbDescriptors.hid;
				return usbDescriptors.hid.bLength;

			case USBDESCR_HID_REPORT:
				usbMsgPtr = (usbMsgPtr_t) usbHidReportDescriptor;
				return usbDescriptors.hid.wDescriptorLength;

			default:
				return 0;
		}
	}
	return 0;
}

