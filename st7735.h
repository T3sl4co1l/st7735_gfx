/*  Adapted from Adafruit 1.8" SPI display library  */

#ifndef ST7735_H_INCLUDED
#define ST7735_H_INCLUDED

#include <inttypes.h>
#include <stdbool.h>
#include <avr/pgmspace.h>
#include <avr/io.h>
#include <avr/cpufunc.h>
#include <util/delay.h>
#include "Image_Compress_Test.h"

#define GRAM_RED_bp			ST7735_GRAM_RED_bp
#define GRAM_GREEN_bp		ST7735_GRAM_GREEN_bp
#define GRAM_RED_bm			ST7735_GRAM_RED_bm
#define GRAM_GREEN_bm		ST7735_GRAM_GREEN_bm
#define GRAM_BLUE_bp		ST7735_GRAM_BLUE_bp
#define GRAM_BLUE_bm		ST7735_GRAM_BLUE_bm

#define ST7735_GRAM_RED_bp		11
#define ST7735_GRAM_RED_bm		( 0x1f << ST7735_GRAM_RED_bp )
#define ST7735_GRAM_GREEN_bp	5
#define ST7735_GRAM_GREEN_bm	( 0x3f << ST7735_GRAM_GREEN_bp )
#define ST7735_GRAM_BLUE_bp		0
#define ST7735_GRAM_BLUE_bm		( 0x1f << ST7735_GRAM_BLUE_bp )

#define HEX_TO_COLOR(x)		( ((((uint32_t)(x)) >> 3) & 0x1f) | (((((uint32_t)(x)) >> 10) & 0x3f) << 5) | (((((uint32_t)(x)) >> 19) & 0x1f) << 11) )

#define SCREEN_WIDTH		160
#define SCREEN_HEIGHT		128

#define WIDTH				SCREEN_WIDTH
#define HEIGHT				SCREEN_HEIGHT

#define ST_CMD_DELAY		0x80	//	special signifier for command lists

/*	Command Description
 *
 *	A command consists of a command byte (D/CX = 0, WR)
 *	followed by zero or more data bytes (D/CX = 1), read
 *	or written.
 *
 *	The commands are listed below in order of command
 *	value, as named in the datasheet.  Interleaved,
 *	a direction flag and a length value.
 *	All data will fit within a uint32_t, so the bit
 *	positions are specified in those terms.
 *	Bits are little-endian, i.e., low byte transmitted
 *	first.  Bit positions are typically used like so:
 *		bitmask = (1 << ST7735_REGISTER_BIT_bp);
 *  To get the bit position within a given byte, say,
 *	use:
 *		bitmask = (1 << (ST7735_REGISTER_BIT_bp % 8));
 *
 *	Each read or write will use ST7735_REGISTER_RDB
 *	or ST7735_REGISTER_WRB bytes for read and write,
 *	respectively, in addition to the command byte.
 *	Where a register is read-only, WRB is 0 (the
 *	command byte write is implicit), and RDB >= 1
 *	(one implicit 'dummy' read is always performed and
 *	discarded; bit positions begin counting with the
 *	second byte read).
 *	When a register is write-only, RDB is 0, and
 *	WRB is the number of parameter bytes written
 *	(0 for commands without parameters, up to 4 for
 *	the longest parameters).
 */

#define ST7735_NOP				0x00
#define ST7735_NOP_WRB			0
#define ST7735_NOP_RDB			0
#define ST7735_SWRESET			0x01
#define ST7735_SWRESET_WRB		0
#define ST7735_SWRESET_RDB		0
#define ST7735_RDDID			0x04
#define ST7735_RDDID_WRB		0
#define ST7735_RDDID_RDB		3
#define ST7735_RDDST			0x09
#define ST7735_RDDST_WRB		0
#define ST7735_RDDST_RDB		4
#define ST7735_RDDPM			0x0A
#define ST7735_RDDPM_WRB		0
#define ST7735_RDDPM_RDB		1
#define ST7735_RDDMADCTL		0x0B
#define ST7735_RDDMADCTL_WRB	0
#define ST7735_RDDMADCTL_RDB	1
#define ST7735_RDDCOLMOD		0x0C
#define ST7735_RDDCOLMOD_WRB	0
#define ST7735_RDDCOLMOD_RDB	1
#define ST7735_RDDIM			0x0D
#define ST7735_RDDIM_WRB		0
#define ST7735_RDDIM_RDB		1
#define ST7735_RDDSM			0x0E
#define ST7735_RDDSM_WRB		0
#define ST7735_RDDSM_RDB		1
#define ST7735_SLPIN			0x10
#define ST7735_SLPIN_WRB		0
#define ST7735_SLPIN_RDB		0
#define ST7735_SLPOUT			0x11
#define ST7735_SLPOUT_WRB		0
#define ST7735_SLPOUT_RDB		0
#define ST7735_PTLON			0x12
#define ST7735_PTLON_WRB		0
#define ST7735_PTLON_RDB		0
#define ST7735_NORON			0x13
#define ST7735_NORON_WRB		0
#define ST7735_NORON_RDB		0
#define ST7735_INVOFF			0x20
#define ST7735_INVOFF_WRB		0
#define ST7735_INVOFF_RDB		0
#define ST7735_INVON			0x21
#define ST7735_INVON_WRB		0
#define ST7735_INVON_RDB		0
#define ST7735_GAMSET			0x26
#define ST7735_GAMSET_WRB		1
#define ST7735_GAMSET_RDB		0
#define ST7735_DISPOFF			0x28
#define ST7735_DISPOFF_WRB		0
#define ST7735_DISPOFF_RDB		0
#define ST7735_DISPON			0x29
#define ST7735_DISPON_WRB		0
#define ST7735_DISPON_RDB		0
#define ST7735_CASET			0x2A
#define ST7735_CASET_WRB		4
#define ST7735_CASET_RDB		0
#define ST7735_RASET			0x2B
#define ST7735_RASET_WRB		4
#define ST7735_RASET_RDB		0
#define ST7735_RAMWR			0x2C
#define ST7735_RAMWR_WRB		1	/* Minimum write size; writes continue until CSX deasserted */
#define ST7735_RAMWR_RDB		0
#define ST7735_RAMRD			0x2E
#define ST7735_RAMRD_WRB		0
#define ST7735_RAMRD_RDB		1	/* Minimum read size; reads continue until CSX deasserted */
#define ST7735_PTLAR			0x30
#define ST7735_PTLAR_WRB		4
#define ST7735_PTLAR_RDB		0
#define ST7735_VSCLLDEF			0x33	/* Vertical Scroll Definition (sumotoy) */
#define ST7735_VSCLLDEF_WRB		4
#define ST7735_VSCLLDEF_RDB		0
#define ST7735_TEOFF			0x34
#define ST7735_TEOFF_WRB		0
#define ST7735_TEOFF_RDB		0
#define ST7735_TEON				0x35
#define ST7735_TEON_WRB			1
#define ST7735_TEON_RDB			0
#define ST7735_MADCTL			0x36
#define ST7735_MADCTL_WRB		1
#define ST7735_MADCTL_RDB		0
#define ST7735_VSSTADRS			0x37	/* Vertical Scroll Start Address (sumotoy) */
#define ST7735_VSSTADRS_WRB		2
#define ST7735_VSSTADRS_RDB		0
#define ST7735_IDMOFF			0x38
#define ST7735_IDMOFF_WRB		0
#define ST7735_IDMOFF_RDB		0
#define ST7735_IDMON			0x39
#define ST7735_IDMON_WRB		0
#define ST7735_IDMON_RDB		0
#define ST7735_COLMOD			0x3A
#define ST7735_COLMOD_WRB		1
#define ST7735_COLMOD_RDB		0
#define ST7735_RDID1			0xDA
#define ST7735_RDID1_WRB		0
#define ST7735_RDID1_RDB		1
#define ST7735_RDID2			0xDB
#define ST7735_RDID2_WRB		0
#define ST7735_RDID2_RDB		1
#define ST7735_RDID3			0xDC
#define ST7735_RDID3_WRB		0
#define ST7735_RDID3_RDB		1
#define ST7735_RDID4			0xDD	/* undocumented */
#define ST7735_RDID4_WRB		0
#define ST7735_RDID4_RDB		1

#define ST7735_RDDID_ID10_bp			0
#define ST7735_RDDID_ID11_bp			1
#define ST7735_RDDID_ID12_bp			2
#define ST7735_RDDID_ID13_bp			3
#define ST7735_RDDID_ID14_bp			4
#define ST7735_RDDID_ID15_bp			5
#define ST7735_RDDID_ID16_bp			6
#define ST7735_RDDID_ID17_bp			7
#define ST7735_RDDID_ID20_bp			0
#define ST7735_RDDID_ID21_bp			1
#define ST7735_RDDID_ID22_bp			2
#define ST7735_RDDID_ID23_bp			3
#define ST7735_RDDID_ID24_bp			4
#define ST7735_RDDID_ID25_bp			5
#define ST7735_RDDID_ID26_bp			6
/* ID27 = 1 */
#define ST7735_RDDID_ID30_bp			0
#define ST7735_RDDID_ID31_bp			1
#define ST7735_RDDID_ID32_bp			2
#define ST7735_RDDID_ID33_bp			3
#define ST7735_RDDID_ID34_bp			4
#define ST7735_RDDID_ID35_bp			5
#define ST7735_RDDID_ID36_bp			6
#define ST7735_RDDID_ID37_bp			7

#define ST7735_RDDST_ST24_bp			0
#define ST7735_RDDST_MH_bp				1
#define ST7735_RDDST_RGB_bp				2
#define ST7735_RDDST_ML_bp				3
#define ST7735_RDDST_MV_bp				4
#define ST7735_RDDST_MX_bp				5
#define ST7735_RDDST_MY_bp				6
#define ST7735_RDDST_BSTON_bp			7
#define ST7735_RDDST_NORON_bp			8
#define ST7735_RDDST_SLOUT_bp			9
#define ST7735_RDDST_PTLON_bp			10
#define ST7735_RDDST_IDMON_bp			11
#define ST7735_RDDST_IFPF0_bp			12
#define ST7735_RDDST_IFPF1_bp			13
#define ST7735_RDDST_IFPF2_bp			14
#define ST7735_RDDST_ST23_bp			15
#define ST7735_RDDST_GCS2_bp			16
#define ST7735_RDDST_TEON_bp			17
#define ST7735_RDDST_DISON_bp			18
#define ST7735_RDDST_ST11_bp			19
#define ST7735_RDDST_ST12_bp			20
#define ST7735_RDDST_INVON_bp			21
#define ST7735_RDDST_ST14_bp			22
#define ST7735_RDDST_VSSON_bp			23
#define ST7735_RDDST_ST0_bp				24
#define ST7735_RDDST_ST1_bp				25
#define ST7735_RDDST_ST2_bp				26
#define ST7735_RDDST_ST3_bp				27
#define ST7735_RDDST_ST4_bp				28
#define ST7735_RDDST_TELOM_bp			29
#define ST7735_RDDST_GCS0_bp			30
#define ST7735_RDDST_GCS1_bp			31

#define ST7735_RDDPM_DISON_bp			2
#define ST7735_RDDPM_NORON_bp			3
#define ST7735_RDDPM_SLPOUT_bp			4
#define ST7735_RDDPM_PTLON_bp			5
#define ST7735_RDDPM_IDMON_bp			6
#define ST7735_RDDPMBSTON_bp			7

#define ST7735_RDDMADCTL_MH_bp			2
#define ST7735_RDDMADCTL_RGB_bp			3
#define ST7735_RDDMADCTL_ML_bp			4
#define ST7735_RDDMADCTL_MV_bp			5
#define ST7735_RDDMADCTL_MX_bp			6
#define ST7735_RDDMADCTL_MY_bp			7

#define ST7735_RDDCOLMOD_IFPF0_bp		0
#define ST7735_RDDCOLMOD_IFPF1_bp		1
#define ST7735_RDDCOLMOD_IFPF2_bp		2

#define ST7735_RDDIM_GCS0_bp			0
#define ST7735_RDDIM_GCS1_bp			1
#define ST7735_RDDIM_GCS2_bp			2
#define ST7735_RDDIM_INVON_bp			5
#define ST7735_RDDIM_D6_bp				6
#define ST7735_RDDIM_VSSON_bp			7

#define ST7735_RDDSM_TELOM_bp			6
#define ST7735_RDDSM_TEON_bp			7

#define ST7735_GAMSET_GC0_bp			0
#define ST7735_GAMSET_GC1_bp			1
#define ST7735_GAMSET_GC2_bp			2
#define ST7735_GAMSET_GC3_bp			3

#define ST7735_CASET_XS8_bp				0
#define ST7735_CASET_XS9_bp				1
#define ST7735_CASET_XS10_bp			2
#define ST7735_CASET_XS11_bp			3
#define ST7735_CASET_XS12_bp			4
#define ST7735_CASET_XS13_bp			5
#define ST7735_CASET_XS14_bp			6
#define ST7735_CASET_XS15_bp			7
#define ST7735_CASET_XS0_bp				8
#define ST7735_CASET_XS1_bp				9
#define ST7735_CASET_XS2_bp				10
#define ST7735_CASET_XS3_bp				11
#define ST7735_CASET_XS4_bp				12
#define ST7735_CASET_XS5_bp				13
#define ST7735_CASET_XS6_bp				14
#define ST7735_CASET_XS7_bp				15
#define ST7735_CASET_XE8_bp				16
#define ST7735_CASET_XE9_bp				17
#define ST7735_CASET_XE10_bp			18
#define ST7735_CASET_XE11_bp			19
#define ST7735_CASET_XE12_bp			20
#define ST7735_CASET_XE13_bp			21
#define ST7735_CASET_XE14_bp			22
#define ST7735_CASET_XE15_bp			23
#define ST7735_CASET_XE0_bp				24
#define ST7735_CASET_XE1_bp				25
#define ST7735_CASET_XE2_bp				26
#define ST7735_CASET_XE3_bp				27
#define ST7735_CASET_XE4_bp				28
#define ST7735_CASET_XE5_bp				29
#define ST7735_CASET_XE6_bp				30
#define ST7735_CASET_XE7_bp				31

#define ST7735_RASET_YS8_bp				0
#define ST7735_RASET_YS9_bp				1
#define ST7735_RASET_YS10_bp			2
#define ST7735_RASET_YS11_bp			3
#define ST7735_RASET_YS12_bp			4
#define ST7735_RASET_YS13_bp			5
#define ST7735_RASET_YS14_bp			6
#define ST7735_RASET_YS15_bp			7
#define ST7735_RASET_YS0_bp				8
#define ST7735_RASET_YS1_bp				9
#define ST7735_RASET_YS2_bp				10
#define ST7735_RASET_YS3_bp				11
#define ST7735_RASET_YS4_bp				12
#define ST7735_RASET_YS5_bp				13
#define ST7735_RASET_YS6_bp				14
#define ST7735_RASET_YS7_bp				15
#define ST7735_RASET_YE8_bp				16
#define ST7735_RASET_YE9_bp				17
#define ST7735_RASET_YE10_bp			18
#define ST7735_RASET_YE11_bp			19
#define ST7735_RASET_YE12_bp			20
#define ST7735_RASET_YE13_bp			21
#define ST7735_RASET_YE14_bp			22
#define ST7735_RASET_YE15_bp			23
#define ST7735_RASET_YE0_bp				24
#define ST7735_RASET_YE1_bp				25
#define ST7735_RASET_YE2_bp				26
#define ST7735_RASET_YE3_bp				27
#define ST7735_RASET_YE4_bp				28
#define ST7735_RASET_YE5_bp				29
#define ST7735_RASET_YE6_bp				30
#define ST7735_RASET_YE7_bp				31

#define ST7735_PTLAR_PSL8_bp			0
#define ST7735_PTLAR_PSL9_bp			1
#define ST7735_PTLAR_PSL10_bp			2
#define ST7735_PTLAR_PSL11_bp			3
#define ST7735_PTLAR_PSL12_bp			4
#define ST7735_PTLAR_PSL13_bp			5
#define ST7735_PTLAR_PSL14_bp			6
#define ST7735_PTLAR_PSL15_bp			7
#define ST7735_PTLAR_PSL0_bp			8
#define ST7735_PTLAR_PSL1_bp			9
#define ST7735_PTLAR_PSL2_bp			10
#define ST7735_PTLAR_PSL3_bp			11
#define ST7735_PTLAR_PSL4_bp			12
#define ST7735_PTLAR_PSL5_bp			13
#define ST7735_PTLAR_PSL6_bp			14
#define ST7735_PTLAR_PSL7_bp			15
#define ST7735_PTLAR_PEL8_bp			16
#define ST7735_PTLAR_PEL9_bp			17
#define ST7735_PTLAR_PEL10_bp			18
#define ST7735_PTLAR_PEL11_bp			19
#define ST7735_PTLAR_PEL12_bp			20
#define ST7735_PTLAR_PEL13_bp			21
#define ST7735_PTLAR_PEL14_bp			22
#define ST7735_PTLAR_PEL15_bp			23
#define ST7735_PTLAR_PEL0_bp			24
#define ST7735_PTLAR_PEL1_bp			25
#define ST7735_PTLAR_PEL2_bp			26
#define ST7735_PTLAR_PEL3_bp			27
#define ST7735_PTLAR_PEL4_bp			28
#define ST7735_PTLAR_PEL5_bp			29
#define ST7735_PTLAR_PEL6_bp			30
#define ST7735_PTLAR_PEL7_bp			31

#define ST7735_TEON_TELOM_bp			0

#define ST7735_MADCTL_MH_bp				2
#define ST7735_MADCTL_RGB_bp			3
#define ST7735_MADCTL_ML_bp				4
#define ST7735_MADCTL_MV_bp				5
#define ST7735_MADCTL_MX_bp				6
#define ST7735_MADCTL_MY_bp				7

#define ST7735_COLMOD_IFPF0_bp			0
#define ST7735_COLMOD_IFPF1_bp			1
#define ST7735_COLMOD_IFPF2_bp			2

#define ST7735_RDID1_ID10_bp			0
#define ST7735_RDID1_ID11_bp			1
#define ST7735_RDID1_ID12_bp			2
#define ST7735_RDID1_ID13_bp			3
#define ST7735_RDID1_ID14_bp			4
#define ST7735_RDID1_ID15_bp			5
#define ST7735_RDID1_ID16_bp			6
#define ST7735_RDID1_ID17_bp			7

#define ST7735_RDID2_ID20_bp			0
#define ST7735_RDID2_ID21_bp			1
#define ST7735_RDID2_ID22_bp			2
#define ST7735_RDID2_ID23_bp			3
#define ST7735_RDID2_ID24_bp			4
#define ST7735_RDID2_ID25_bp			5
#define ST7735_RDID2_ID26_bp			6
#define ST7735_RDID2_ID27_bp			7

#define ST7735_RDID3_ID30_bp			0
#define ST7735_RDID3_ID31_bp			1
#define ST7735_RDID3_ID32_bp			2
#define ST7735_RDID3_ID33_bp			3
#define ST7735_RDID3_ID34_bp			4
#define ST7735_RDID3_ID35_bp			5
#define ST7735_RDID3_ID36_bp			6
#define ST7735_RDID3_ID37_bp			7

// some flags for initR() :(
#define INITR_GREENTAB		0x0
#define INITR_18GREENTAB	INITR_GREENTAB

#define ST7735_MADCTL_BGR	0x08
#define ST7735_MADCTL_MH	0x04

#define ST7735_FRMCTR1		0xB1
#define ST7735_FRMCTR2		0xB2
#define ST7735_FRMCTR3		0xB3
#define ST7735_INVCTR		0xB4
#define ST7735_DISSET5		0xB6

#define ST7735_PWCTR1		0xC0
#define ST7735_PWCTR2		0xC1
#define ST7735_PWCTR3		0xC2
#define ST7735_PWCTR4		0xC3
#define ST7735_PWCTR5		0xC4
#define ST7735_VMCTR1		0xC5

#define ST7735_PWCTR6		0xFC

#define ST7735_GMCTRP1		0xE0
#define ST7735_GMCTRN1		0xE1


void ST7735_displayInit(const uint8_t* addr);
void delay_ms(uint16_t ms);
void ST7735_beginCommand(void);
void ST7735_beginData(void);
void ST7735_commandToData(void);
void ST7735_dataToCommand(void);
uint8_t ST7735_sendByte(uint8_t b);
void ST7735_endTransaction(void);
void ST7735_sendCommand(uint8_t cmd);
void ST7735_sendData(uint8_t cmd);
void initLcd(void);
void setScreenRegion(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void writePixel(uint16_t c);
void drawfillRectangle(uint16_t colr, uint16_t xStart, uint16_t yStart,
				uint16_t width, uint16_t height);
#ifdef IMAGE_COMPRESS_TEST_H_INCLUDED
const uint8_t PROGMEM ImageData[1 + IMAGE_COMPRESS_TEST_PALETTE_LEN * 2 + IMAGE_COMPRESS_TEST_CMD_LEN];
#endif // IMAGE_COMPRESS_TEST_H_INCLUDED
void drawImage(const uint8_t* img, const uint8_t x, const uint8_t y);

#endif // ST7735_H_INCLUDED
