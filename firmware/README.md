# Firmware

It is written in C using [V-USB library](https://github.com/obdev/v-usb).

## Compilation
You need *avr-gcc* to build it. There is standard *Makefile* attached.

On Ubuntu install:
```
sudo apt install gcc-avr binutils-avr avr-libc
```

Then build it:
```
make
```

## Flashing
You need [micronucleus](https://github.com/micronucleus/micronucleus) programmer to flash over USB, or anything that is able flash your ATtiny.

Flashing NES version with micronucleus:
```
make flash-nes
```
Run this command, then connect your board to USB.

For SNES version:
```
make flash-snes
```
