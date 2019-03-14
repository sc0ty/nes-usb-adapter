# NES USB adapter
Nintendo [NES](https://en.wikipedia.org/wiki/Nintendo_Entertainment_System)/[SNES](https://en.wikipedia.org/wiki/Super_Nintendo_Entertainment_System) controller USB adapter.

It is recognized as standard USB HID joystick or keyboard. It should work without any additional drivers on Windows/Mac/Linux/whatever.

It uses DigiSpark board, see [hardware](hardware/README.md) for details.

Compilation and flashing process is described in [firmware](firmware/README.md).

⚠ **SNES version is NOT tested!** ⚠ I don't have this controller.
It is pretty similar to NES one therefore it shouldn't be issues with it, maybe some minors.
If you made it, please let me know if it works.

## Usage
It is plug and play and should work from the start.

### Mode selection
There are four modes of operation selected with key combinations on your controller.

Key combination    | Mode                  | Description
-------------------|-----------------------|---------------
START+SELECT+UP    | default joystick      | 4-buttons (8-buttons for SNES) game controller, default mode
START+SELECT+DOWN  | 10-buttons joystick   | SELECT and START are mapped as button 9 and 10. Games are expecting to have this buttons there, so there is no need to remap them in game. There are some buttons unused.
START+SELECT+LEFT  | keyboard              | for games without gamepad support (or only supporting Microsoft Xinput controllers like Limbo)
START+SELECT+RIGHT | keyboard (alternative)| with B mapped as up arrow. It is for games controlled with arrows and one extra button, where up arrow is for jump. Like Limbo.

Key mappings for keyboard mode (* only for SNES):

Gamepad key   | Keyboard key | Comments
--------------|--------------|----------
arrows        | arrows       |
A             | left CTRL    |
B             | left Shift   |
START         | Enter        |
SELECT        | Esc          |
X             | M            | SNES only
Y             | N            | SNES only
left trigger  | J            | SNES only
right trigger | K            | SNES only

### Key swapping
In any mode buttons A and B could be swapped with another key combination.
For SNES, buttons X and Y also could be swapped.
In 10-buttons joystick mode SNES triggers could be configured as triggers or shoulder buttons.

Key combination                | Description
-------------------------------|----------------------------------
START + SELECT + A             | Disable swapping A / B
START + SELECT + B             | Enable swapping A / B
START + SELECT + X             | Disable swapping X / Y (SNES only)
START + SELECT + Y             | Enable swapping X / Y (SNES only)
START + SELECT + left trigger  | Use as shoulder buttons (SNES only)
START + SELECT + right trigger | Use as triggers (SNES only)
