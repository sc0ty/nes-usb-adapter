/* USB Descriptors and configuration */

#include "usb_descriptors.h"
#include "usbdrv.h"
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stddef.h>


struct UsbDeviceDescriptor usbDeviceDescriptor = {
	.bLength                    = sizeof(struct UsbDeviceDescriptor),
	.bDescriptorType            = USBDESCR_DEVICE,
	.bcdUSB                     = 0x0110,
	.bDeviceClass               = 0,
	.bDeviceSubClass            = 0,
	.bDeviceProtocol            = 0,
	.bMaxPacketSize             = 8,
	.idVendor                   = 0,
	.idProduct                  = 0,
	.bcdDevice                  = 0x0100,
	.iManufacturer              = 1,
	.iProduct                   = 2,
	.iSerialNumber              = 0,
	.bNumConfigurations         = 1,
};

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
		.bInterfaceProtocol     = 1,
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
const char *usbProductName = NULL;


void usbConfig(struct UsbDeviceId deviceId, const char *productName,
		void *hidDescriptor, uint16_t hidDescriptorLen)
{
	cli();
	usbDeviceDisconnect();
	_delay_ms(500);

	usbDeviceDescriptor.idVendor  = deviceId.vendor;
	usbDeviceDescriptor.idProduct = deviceId.product;

	usbProductName = productName;

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

static struct UsbStringDescriptor *getStringDescriptor(const char *str)
{
	static struct UsbStringDescriptor descriptor = {
		.bDescriptorType            = USBDESCR_STRING,
	};

	uint8_t i;
	for (i = 0; str[i] && i < 20; i++)
		descriptor.bString[i] = str[i];

	descriptor.bLength = 2 * i + 2;
	return &descriptor;
}

usbMsgLen_t usbFunctionDescriptor(struct usbRequest *rq)
{
	struct UsbStringDescriptor *stringDescriptor;

	if (rq->bRequest == USBRQ_GET_DESCRIPTOR)
	{
		switch (rq->wValue.bytes[1])
		{
			case USBDESCR_DEVICE:
				usbMsgPtr = (usbMsgPtr_t) &usbDeviceDescriptor;
				return sizeof(usbDeviceDescriptor);

			case USBDESCR_CONFIG:
				usbMsgPtr = (usbMsgPtr_t) &usbDescriptors;
				return sizeof(usbDescriptors);

			case USBDESCR_HID:
				usbMsgPtr = (usbMsgPtr_t) &usbDescriptors.hid;
				return sizeof(usbDescriptors);

			case USBDESCR_HID_REPORT:
				usbMsgPtr = (usbMsgPtr_t) usbHidReportDescriptor;
				return usbDescriptors.hid.wDescriptorLength;

			case USBDESCR_STRING:
				switch (rq->wValue.bytes[0])
				{
					case 2: // product string
					default:
						stringDescriptor = getStringDescriptor(usbProductName);
						usbMsgPtr = (usbMsgPtr_t) stringDescriptor;
						return stringDescriptor->bLength;
				};

			default:
				return 0;
		}
	}
	return 0;
}

