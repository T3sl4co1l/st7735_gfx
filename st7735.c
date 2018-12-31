#include "st7735.h"

//	Rather than a bazillion writecommand() and writedata() calls, screen
//	initialization commands and arguments are organized in these tables
//	stored in PROGMEM.	The table may look bulky, but that's mostly the
//	formatting -- storage-wise this is hundreds of bytes more compact
//	than the equivalent code.	Companion function follows.

//	Init for 7735R, part 1 (red or green tab)
const uint8_t ST7735_Rcmd1[] PROGMEM = {
	15,									//	15 commands in list:
	ST7735_SWRESET,	ST_CMD_DELAY,		//	1: Software reset, 0 args, w/delay
		150,							//		150 ms delay
	ST7735_SLPOUT,	ST_CMD_DELAY,		//	2: Out of sleep mode, 0 args, w/delay
		255,							//		500 ms delay
	ST7735_FRMCTR1,	3,					//	3: Frame rate ctrl - normal mode, 3 args:
		0x01, 0x2C, 0x2D,				//		Rate = fosc/(1x2+40) * (LINE+2C+2D)
	ST7735_FRMCTR2,	3,					//	4: Frame rate control - idle mode, 3 args:
		0x01, 0x2C, 0x2D,				//		Rate = fosc/(1x2+40) * (LINE+2C+2D)
	ST7735_FRMCTR3,	6,					//	5: Frame rate ctrl - partial mode, 6 args:
		0x01, 0x2C, 0x2D,				//		Dot inversion mode
		0x01, 0x2C, 0x2D,				//		Line inversion mode
	ST7735_INVCTR,	1,					//	6: Display inversion ctrl, 1 arg, no delay:
		0x07,							//		No inversion
	ST7735_PWCTR1,	3,					//	7: Power control, 3 args, no delay:
		0xA2,
		0x02,							//		-4.6V
		0x84,							//		AUTO mode
	ST7735_PWCTR2,	1,					//	8: Power control, 1 arg, no delay:
		0xC5,							//		VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD
	ST7735_PWCTR3,	2,					//	9: Power control, 2 args, no delay:
		0x0A,							//		Opamp current small
		0x00,							//		Boost frequency
	ST7735_PWCTR4,	2,					//	10: Power control, 2 args, no delay:
		0x8A,							//		BCLK/2, Opamp current small & Medium low
		0x2A,
	ST7735_PWCTR5,	2,					//	11: Power control, 2 args, no delay:
		0x8A, 0xEE,
	ST7735_VMCTR1,	1,					//	12: Power control, 1 arg, no delay:
		0x0E,
	ST7735_INVOFF,	0,					//	13: Don't invert display, no args, no delay
	ST7735_MADCTL,	1,					//	14: Memory access control (directions), 1 arg:
//		0xC8,							//		row addr/col addr, bottom to top refresh
		0x50,							//		[determined empirically for application]
	ST7735_COLMOD,	1,					//	15: set color mode, 1 arg, no delay:
		0x05							//		16-bit color
};

//	Init for 7735R, part 2 (green tab only)
const uint8_t ST7735_Rcmd2green[] PROGMEM = {
	2,									//	2 commands in list:
	ST7735_CASET,	4,					//	1: Column addr set, 4 args, no delay:
		0x00, 0x02,						//		XSTART = 0
		0x00, 0x7F + 0x02,				//		XEND = 127
	ST7735_RASET,	4,					//	2: Row addr set, 4 args, no delay:
		0x00, 0x01,						//		XSTART = 0
		0x00, 0x9F + 0x01				//		XEND = 159
};

//	Init for 7735R, part 3 (red or green tab)
const uint8_t ST7735_Rcmd3[] PROGMEM = {
	4,									//	4 commands in list:
	ST7735_GMCTRP1,	16,					//	1: Magical unicorn dust, 16 args, no delay:
		0x02, 0x1c, 0x07, 0x12,
		0x37, 0x32, 0x29, 0x2d,
		0x29, 0x25, 0x2B, 0x39,
		0x00, 0x01, 0x03, 0x10,
	ST7735_GMCTRN1,	16,					//	2: Sparkles and rainbows, 16 args, no delay:
		0x03, 0x1d, 0x07, 0x06,
		0x2E, 0x2C, 0x29, 0x2D,
		0x2E, 0x2E, 0x37, 0x3F,
		0x00, 0x00, 0x02, 0x10,
	ST7735_NORON,	ST_CMD_DELAY,		//	3: Normal display on, no args, w/delay
		10,								//		10 ms delay
	ST7735_DISPON,	ST_CMD_DELAY,		//	4: Main screen turn on, no args w/delay
		100								//		100 ms delay
};

#ifdef IMAGE_COMPRESS_TEST_H_INCLUDED
const uint8_t ImageData[] PROGMEM = {
	(IMAGE_COMPRESS_TEST_PALETTE_LEN & 0x00ff),
	IMAGE_COMPRESS_TEST_PAL,
	IMAGE_COMPRESS_TEST_CMDS
};
#endif // IMAGE_COMPRESS_TEST_H_INCLUDED


void ST7735_displayInit(const uint8_t* addr) {
	uint8_t numCommands, numArgs;
	uint16_t ms;

	if (addr == 0) {
		return;
	}
	numCommands = pgm_read_byte(addr++);	//	Number of commands to follow
	ST7735_beginCommand();
	while (numCommands--) {					//	For each command...
		ST7735_dataToCommand();
		ST7735_sendByte(pgm_read_byte(addr++));
		numArgs = pgm_read_byte(addr++);	//	Number of args to follow
		ms = numArgs & ST_CMD_DELAY;		//	If high bit set, delay follows args
		numArgs &= ~ST_CMD_DELAY;			//	Mask out delay bit
		ST7735_commandToData();
		while (numArgs--) {					//	For each argument...
			ST7735_sendByte(pgm_read_byte(addr++));	//	Read and issue argument
		}
		if (ms) {
			ms = pgm_read_byte(addr++);		//	Read post-command delay time (ms)
			if (ms == 255) {				//	If 255, delay for 500 ms
				ms = 500;
			}
			delay_ms(ms);
		}
	}
	ST7735_endTransaction();
}

void delay_ms(uint16_t ms) {

	while (ms--) {
		_delay_ms(1);
	}

}

inline void ST7735_beginCommand(void) {
	SPI_TXWAIT();
	SPI_DC_CMD();
	SPI_CSON();
}
inline void ST7735_beginData(void) {
	SPI_TXWAIT();
	SPI_DC_DATA();
	SPI_CSON();
}

inline void ST7735_commandToData(void) {
	SPI_TXWAIT();
	SPI_DC_DATA();
}

inline void ST7735_dataToCommand(void) {
	SPI_TXWAIT();
	SPI_DC_CMD();
}

uint8_t ST7735_sendByte(uint8_t b) {
	SPI_TXWAIT();
	SPI_DATA = b;
	return SPI_DATA;
}

inline void ST7735_endTransaction(void) {
	SPI_TXWAIT();
	SPI_CSOFF();
}

void ST7735_sendCommand(uint8_t cmd) {

	ST7735_beginCommand();
	ST7735_sendByte(cmd);
	ST7735_endTransaction();

}

void ST7735_sendData(uint8_t cmd) {

	ST7735_beginData();
	ST7735_sendByte(cmd);
	ST7735_endTransaction();

}

void initLcd(void) {

	PORT_SPI_RS.OUTCLR = BIT_SPI_RS;	//	Reset display
	_delay_ms(5);
	PORT_SPI_RS.OUTSET = BIT_SPI_RS;
	_delay_ms(10);
	ST7735_displayInit(ST7735_Rcmd1);
	ST7735_displayInit(ST7735_Rcmd2green);
	ST7735_displayInit(ST7735_Rcmd3);
}

/**
 *	Sets the active (drawing) screen region, inclusive.
 *	Region wraps bottom-to-top and right-to-left,
 *	Or horizontal than vertical, depending on
 *	orientation state (MADCTL).
 *	@param x1	Left edge
 *	@param y1	Top edge
 *	@param x2	Right edge
 *	@param y2	Bottom edge
 */
void setScreenRegion(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {

	ST7735_beginCommand();
	ST7735_sendByte(ST7735_RASET);
	ST7735_commandToData();
	ST7735_sendByte(x1 >> 8);
	ST7735_sendByte(x1 & 0xff);
	ST7735_sendByte(x2 >> 8);
	ST7735_sendByte(x2 & 0xff);

	ST7735_dataToCommand();
	ST7735_sendByte(ST7735_CASET);
	ST7735_commandToData();
	ST7735_sendByte(y1 >> 8);
	ST7735_sendByte(y1 & 0xff);
	ST7735_sendByte(y2 >> 8);
	ST7735_sendByte(y2 & 0xff);
	ST7735_endTransaction();

}

void writePixel(uint16_t c) {

	ST7735_beginCommand();
	ST7735_sendByte(ST7735_RAMWR);
	ST7735_commandToData();
	ST7735_sendByte(c >> 8);
	ST7735_sendByte(c & 0xff);
	ST7735_endTransaction();

}

/**
 * Draws a filled rectangle at the specified location, in the
 * specified color.
 */
void drawfillRectangle(uint16_t colr, uint16_t xStart, uint16_t yStart,
				uint16_t width, uint16_t height) {

	uint16_t pixels;

	pixels = width * height;

	//	Set column (X) address start and end
	setScreenRegion(xStart, yStart, xStart + width - 1, yStart + height - 1);

	ST7735_beginCommand();
	ST7735_sendByte(ST7735_RAMWR);
	ST7735_commandToData();

	do {
		ST7735_sendByte(colr >> 8);
		ST7735_sendByte(colr & 0xff);
	} while (--pixels);
	ST7735_endTransaction();

}

/**
 *	Draw image starting at the specified location.
 *	(Use the ST7735 Compressor tool to generate code.)
 *	Overdraw past the edge of the screen is discarded.
 *	@param img	PROGMEM image (encoded) buffer to draw
 *	@param x	x and...
 *	@param y	y coordinate to draw image at, referenced to top-left corner
 */
void drawImage(const uint8_t* img, const uint8_t x, const uint8_t y) {

	const uint8_t* palette;
	uint8_t cmd;	//	Command/instruction byte
	uint8_t xStart, yStart, width, height;	//	Location (or length) parameters
	uint16_t area;	//	number of bitmap pixels to read
	uint16_t colr;	//	Color to draw (5-6-5 format)

	palette = img + 1;			//	get palette offset and
	area = pgm_read_byte(img);	//	  (borrow area for this calculation)
	if (area == 0) area = 256;
	img += 1 + 2 * area;		//	skip over palette

	do {
		//	Get instruction
		cmd = pgm_read_byte(img++);
		//	Process single-byte commands
		if ((cmd & 0x0f) == 0) {			//	No Operation
			width = 0;
		} else if (cmd == 0xff) {		//	Terminate
			return;
		} else {
			//	Read multi-byte command parameters
			if (!(cmd & 0x10)) {		//	No bitmap flag, read color
				colr = pgm_read_word(palette + 2 * pgm_read_byte(img++));
			}
			//	Get xStart, yStart
			xStart = pgm_read_byte(img++);
			yStart = pgm_read_byte(img++);
			width = 1; height = 1;		//	Assume pixel (implicit width and height)...
			if ((cmd & 0x0f) != 1) {		//	Not pixel: Get width or more
				width = pgm_read_byte(img++);	//	Assume horizontal line...
				if (width == 0) width = 1;	//	Quick bounds check just in case...
				if ((cmd & 0x0f) == 3) {	//	vertical line: swap width, height
					height = width;
					width = 1;
				} else if ((cmd & 0x0f) == 5) {	//	rectangle: get height too
					height = pgm_read_byte(img++);
					if (height == 0) height = 1;
				}
			}
		}

		//	Decoded something?  Write to display
		if (width) {
			//	Draw Bitmap Rectangle
			area = width * height;
			setScreenRegion(x + xStart, y + yStart, x + xStart + width - 1, y + yStart + height - 1);
			ST7735_beginCommand();
			ST7735_sendByte(ST7735_RAMWR);
			ST7735_commandToData();
			do {
				if (cmd & 0x10) {	//	Bitmap flag, gotta get pixel data
					colr = pgm_read_word(palette + 2 * pgm_read_byte(img++));
				}
				ST7735_sendByte(colr >> 8);
				ST7735_sendByte(colr & 0xff);
			} while (--area);
			ST7735_endTransaction();
		}
	} while (1);

}
