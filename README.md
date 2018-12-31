# st7735_gfx
ST7735 LCD Display Driver

Adapted from https://github.com/adafruit/Adafruit-ST7735-Library and related resources.  Heavily refactored to C, and to my design style.  (The init data/code is about all that's copied; the rest has been heavily rewritten, or composed from scratch, taking guidance from here.)

## Interface
The `SPI_` macros are provided by your project header; they do what they say.  Example (for XMega64 and the like):
```
#define PIN_SPI_CS		PIN4
#define BIT_SPI_CS		(1 << PIN4)
#define PORT_SPI_CS		PORTD
...
#define SPI_PORT		SPID
#define SPI_DATA		SPID.DATA
#define SPI_CSON()		PORT_SPI_CS.OUTCLR = BIT_SPI_CS
#define SPI_CSOFF()		PORT_SPI_CS.OUTSET = BIT_SPI_CS
#define SPI_DC_DATA()		PORT_SPI_DC.OUTSET = BIT_SPI_DC
#define SPI_DC_CMD()		PORT_SPI_DC.OUTCLR = BIT_SPI_DC
#define SPI_TXWAIT()		while (!(SPI_PORT.STATUS & SPI_IF_bm) && (SPI_PORT.CTRL & SPI_ENABLE_bm))
```

All (currently known?) display registers and bits have been named.  This is formatted consistent with Atmel's MCU header style, so should be familiar to the bare-ish-bones AVR user (if rather messy to an Arduino user.. who probably shouldn't be looking at this, anyway?).  The one customization is that the length of a register is variable, and not apparent from its name -- the `_WRB` and `_RDB` (Write Bytes and Read Bytes, respectively) macros supply this.

initLcd() is the initializer entry point.  ST7735_displayInit() could be folded into it since it isn't used elsewhere and I'm not working with other display types at this time -- but if you are, add them here.  The Adafruit libraries show several supported variants, which should be easy to plug in with few changes.

There are no display-read functions implemented (except for sendByte), because my implementation ran out of pins, and because strapping SDA (bidirectional) to MOSI /and/ MISO is a bit of a juggle.  Since VRAM is much larger than SRAM on most small MCUs (such as AVRs), this would be a convenient (if somewhat slow) way to implement alpha blending, say, or anything else that depends on a frame buffer.

The big step that brought on this repo is the drawImage() function.  Like the init system, this processes a PROGMEM array as a series of variable-length commands.  The format is like an RLE (run length encoded) image, but rather than a linear (scanline or string based) code, it's 2-dimensional -- ultimately because the ST7735 only has one set-region command and no RAM address control, so it's optimal to write blocks at a time.

So, I embarked on writing an encoder, which took about two weeks to complete.  Run it here:
https://htmlpreview.github.io/?https://github.com/T3sl4co1l/st7735_gfx/blob/master/compr.html

This page contains the JS encoder, an HTML interface, and documents the format.  Output is verbose (English) text, and C header output.  TODO: the binary output doesn't actually download anything right now.  File download function is borrowed from: https://github.com/eligrey/FileSaver.js/

A greedy algorithm is used, and has compression ratio comparable to Windows RLE (an example, and its H output, is provided here).  I'm quite satisfied with the results!

A number of optimizations could be made, give or take towards file size, or draw speed (which is dominated by the amount of SPI traffic, especially setRegion commands), but aren't going to be easy to patch into the middle of the encoder mega-function.
