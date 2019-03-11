/* USB Descriptors and configuration */

#ifndef __DESCRIPTORS_H__
#define __DESCRIPTORS_H__

#include <stdint.h>


struct UsbDeviceDescriptor
{
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint16_t bcdUSB;
	uint8_t bDeviceClass;
	uint8_t bDeviceSubClass;
	uint8_t bDeviceProtocol;
	uint8_t bMaxPacketSize;
	uint16_t idVendor;
	uint16_t idProduct;
	uint16_t bcdDevice;
	uint8_t iManufacturer;
	uint8_t iProduct;
	uint8_t iSerialNumber;
	uint8_t bNumConfigurations;
};

struct UsbConfigurationDescriptor
{
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint16_t wTotalLength;
	uint8_t bNumInterfaces;
	uint8_t bConfigurationValue;
	uint8_t iConfiguration;
	uint8_t bmAttributes;
	uint8_t bMaxPower;
};

struct UsbInterfaceDescriptor
{
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bInterfaceNumber;
	uint8_t bAlternateSetting;
	uint8_t bNumEndpoints;
	uint8_t bInterfaceClass;
	uint8_t bInterfaceSubClass;
	uint8_t bInterfaceProtocol;
	uint8_t iInterface;
};

struct UsbHidDescriptor
{
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint16_t bcdHID;
	uint8_t bCountryCode;
	uint8_t bNumDescriptors;
	uint8_t bDescriptorType_HID;
	uint16_t wDescriptorLength;
};

struct UsbEndpointDescriptor
{
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bEndpointAddress;
	uint8_t bmAttributes;
	uint16_t wMaxPacketSize;
	uint8_t bInterval;
};

struct UsbDescriptors
{
	struct UsbConfigurationDescriptor configuration;
	struct UsbInterfaceDescriptor interface;
	struct UsbHidDescriptor hid;
	struct UsbEndpointDescriptor endpoint;
};

struct UsbStringDescriptor
{
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint16_t bString[20];
};

enum UsbDeviceMode
{
	USB_DEVICE_JOYSTICK,
	USB_DEVICE_KEYBOARD,
};

void usbConfig(enum UsbDeviceMode mode, const char *productName,
		const void *hidDescriptor, uint16_t hidDescriptorLen);

#endif
