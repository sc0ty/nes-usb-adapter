CC     = avr-gcc
OBJCPY = avr-objcopy
FLASH  = micronucleus --run


CFLAGS = \
		 -g \
		 -Os \
		 --std=c99 \
		 -Wall \
		 -Wextra \
		 -Wpedantic \
		 -ffunction-sections \
		 -fdata-sections \
		 -mmcu=attiny85 \
		 -DF_CPU=16500000L \
		 -I. \
		 -Iusbdrv \
		 -Ilibs-device \

CXXFLAGS = $(CFLAGS)

LDFLAGS  = \
		   -Os \
		   -Wl,--gc-sections \
		   -mmcu=attiny85 \


TARGETS = \
		  nes.hex \
		  snes.hex \

SOURCES = \
		  serial_pad.c \
		  usb_descriptors.c \
		  usbdrv/usbdrv.c \
		  usbdrv/usbdrvasm.S \
		  libs-device/osccal.c \

HEADERS = \
		  serial_pad.h \
		  usb_descriptors.h \
		  hid_pad4.h \
		  hid_pad8.h \
		  hid_pad10.h \
		  usbconfig.h \


ELFS     = $(TARGETS:.hex=.elf)
OBJECTS  = $(addsuffix .o,$(SOURCES))
TOBJECTS = $(TARGETS:.hex=.c.o)


all: $(TARGETS)

clean:
	$(RM) $(TARGETS)
	$(RM) $(ELFS)
	$(RM) $(TOBJECTS)
	$(RM) $(OBJECTS)


$(OBJECTS):
$(TOBJECTS):
$(ELFS):

%.c.o: %.c $(HEADERS)
	$(CC) -c $(CFLAGS) $< -o $@

%.S.o: %.S $(HEADERS)
	$(CC) -c -x assembler-with-cpp $(CFLAGS) $< -o $@


%.elf: %.c.o $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^
	#avr-size -A $@

%.hex: %.elf
	$(OBJCPY) -O ihex -R .eeprom $< $@


flash-%: %.hex
	$(FLASH) $<


.PHONY: all clean flash-%

