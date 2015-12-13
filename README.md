# led-light-firmware
led-light-firmware

Firmware for pic16f1827 microcontoller based led light
It's using 5 multicolor CREE MC-E LEDs and 6 4000k white MC-E LEDs.

The firmware supports 
- RC5 remote control
- white light mode with different color temperatures (cold white, standard white and warm white)
- fixed color mode with 9 colors directly accessiable from remote control
- color fade mode
- manual color mode to manually set up the four available channels (r-g-b-w). Memory to store 10 colors.
- startup modes: last used and white light.

I'm using
- mplab IDE early 2011 version
- hitech software picc 9.81 c compiler. version with working opitmization needed as otherwise device memory isn't sufficient.

haven't tried mplab-x because... do not change running systems.
haven't tried xc compiler because... do not change running systems, especially if updates are expensive :-)
