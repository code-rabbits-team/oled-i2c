/*
  OzOLED.cpp - 0.96' I2C 128x64 OLED Driver Library
  2014 Copyright (c) OscarLiang.net  All right reserved.

  Author: Oscar Liang

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

*/

#include "oled-i2c.h"
#include <Wire.h>
#include <avr/pgmspace.h>
#include "font.h"

OLEDI2C::OLEDI2C() : OLEDI2C(128, 64) {}

OLEDI2C::OLEDI2C(uint8_t width, uint8_t height)
	: OLEDI2C(Wire, width, height) {}

OLEDI2C::OLEDI2C(TwoWire wire, uint8_t width, uint8_t height)
	: _wire(wire), _width(width), _height(height) {}

void OLEDI2C::sendCommand(byte command)
{
	_wire.beginTransmission(OLED_ADDRESS);
	_wire.write(OzOLED_COMMAND_MODE);
	_wire.write(command);
	_wire.endTransmission();
}

void OLEDI2C::sendData(byte data)
{

	_wire.beginTransmission(OLED_ADDRESS);
	_wire.write(OzOLED_DATA_MODE);
	_wire.write(data);
	_wire.endTransmission();
}

size_t OLEDI2C::print(char ch)
{
	// Ignore unused ASCII characters. Modified the range to support multilingual characters.
	if (ch < 32 || ch > 127)
	{
		ch = '*'; // star - indicate characters that can't be displayed
	}

	for (byte i = 0; i < 8; i++)
	{
		// read bytes from code memory
		sendData(pgm_read_byte(&asciiFont[ch - 32][i])); // font array starts at 0, ASCII starts at 32. Hence the translation
	}
}

size_t OLEDI2C::print(char ch, uint8_t x, uint8_t y)
{
	if (x >= _width || y >= _height)
	{
		return;
	}

	setCursorXY(x, y);
	print(ch);
}

size_t OLEDI2C::print(const char *str)
{
	int i = -1;
	int count = 0;
	char ch;
	while ((ch = str[++i]) != '\0')
	{
		count += print(ch);
	}

	return i;
}

size_t OLEDI2C::print(const char *str, uint8_t x, uint8_t y)
{
	if (x >= _width || y >= _height)
	{
		return;
	}

	setCursorXY(x, y);
	return print(str);
}

byte OLEDI2C::printNumber(long long_num, byte X, byte Y)
{
	if (X < _width)
	{
		setCursorXY(X, Y);
	}

	byte char_buffer[10] = "";
	byte i = 0;
	byte f = 0; // number of characters

	if (long_num < 0)
	{
		f++;
		print('-');
		long_num = -long_num;
	}
	else if (long_num == 0)
	{
		f++;
		print('0');
		return f;
	}

	while (long_num > 0)
	{

		char_buffer[i++] = long_num % 10;
		long_num /= 10;
	}

	f += i;
	for (; i > 0; i--)
	{
		print('0' + char_buffer[i - 1]);
	}

	return f;
}

byte OLEDI2C::printNumber(float float_num, byte prec, byte X, byte Y)
{
	if (X < _width)
	{
		setCursorXY(X, Y);
	}

	// prec - 6 maximum

	byte num_int = 0;
	byte num_frac = 0;
	byte num_extra = 0;

	long d = float_num;		 // get the integer part
	float f = float_num - d; // get the fractional part

	if (d == 0 && f < 0.0)
	{
		print('-');
		num_extra++;
		print('0');
		num_extra++;
		f *= -1;
	}
	else if (d < 0 && f < 0.0)
	{
		num_int = printNumber(d); // count how many digits in integer part
		f *= -1;
	}
	else
	{
		num_int = printNumber(d); // count how many digits in integer part
	}

	// only when fractional part > 0, we show decimal point
	if (f > 0.0)
	{
		print('.');
		num_extra++;

		long f_shift = 1;

		if (num_int + prec > 8)
		{
			prec = 8 - num_int;
		}

		for (byte j = 0; j < prec; j++)
		{
			f_shift *= 10;
		}

		num_frac = printNumber((long)(f * f_shift)); // count how many digits in fractional part
	}

	return num_int + num_frac + num_extra;
}

void OLEDI2C::printBigNumber(const char *number, byte X, byte Y, byte numChar)
{
	// big number pixels: 24 x 32

	// Y - page
	byte column = 0;
	byte count = 0;

	while (number[count] && count < numChar)
	{
		setCursorXY(X, Y);

		for (byte i = 0; i < 96; i++)
		{
			// if character is not "0-9" or ':'
			if (number[count] < 48 || number[count] > 58)
			{
				sendData(0);
			}
			else
			{
				sendData(pgm_read_byte(&bigNumbers[number[count] - 48][i]));
			}

			if (column >= 23)
			{
				column = 0;
				setCursorXY(X, ++Y);
			}
			else
			{
				column++;
			}
		}

		count++;

		X = X + 3;
		Y = Y - 4;
	}
}

void OLEDI2C::drawBitmap(const byte *bitmaparray, byte X, byte Y, byte width, byte height)
{

	// max width = 16
	// max height = 8

	setCursorXY(X, Y);

	byte column = 0;
	for (int i = 0; i < width * 8 * height; i++)
	{
		sendData(pgm_read_byte(&bitmaparray[i]));

		if (++column == width * 8)
		{
			column = 0;
			setCursorXY(X, ++Y);
		}
	}
}

void OLEDI2C::init()
{
	_wire.begin();

	// upgrade to 400KHz! (only use when your other i2c device support this speed)
	if (I2C_400KHZ)
	{
		TWBR = 12;
	}

	setPowerOff(); // display off
	delay(10);
	setPowerOn(); // display on
	delay(10);
	setNormalDisplay(); // default Set Normal Display
	setPageMode();		// default addressing mode
	clearDisplay();
	setCursorXY(0, 0);
}

void OLEDI2C::setCursorXY(byte X, byte Y)
{
	// Y - 1 unit = 1 page (8 pixel rows)
	// X - 1 unit = 8 pixel columns

	sendCommand(0x01 + (8 * X & 0x0F));		   // set column lower address
	sendCommand(0x10 + ((8 * X >> 4) & 0x0F)); // set column higher address
	sendCommand(0xB0 + Y);					   // set page address
}

void OLEDI2C::clearDisplay()
{
	for (byte page = 0; page < 8; page++)
	{
		setCursorXY(0, page);
		for (byte column = 0; column < _width; column++)
		{
			// clear all columns
			sendData(0);
		}
	}

	setCursorXY(0, 0);
}

void OLEDI2C::setInverseDisplay()
{
	sendCommand(OzOLED_CMD_INVERSE_DISPLAY);
}

void OLEDI2C::setNormalDisplay()
{
	sendCommand(OzOLED_CMD_NORMAL_DISPLAY);
}

void OLEDI2C::setPowerOff()
{
	sendCommand(OzOLED_CMD_DISPLAY_OFF);
}

void OLEDI2C::setPowerOn()
{
	sendCommand(OzOLED_CMD_DISPLAY_ON);
}

void OLEDI2C::setBrightness(byte Brightness)
{
	sendCommand(OzOLED_CMD_SET_BRIGHTNESS);
	sendCommand(Brightness);
}

void OLEDI2C::setPageMode()
{
	addressingMode = PAGE_ADDRESSING;
	sendCommand(0x20);			  // set addressing mode
	sendCommand(PAGE_ADDRESSING); // set page addressing mode
}

void OLEDI2C::setHorizontalMode()
{
	addressingMode = HORIZONTAL_ADDRESSING;
	sendCommand(0x20);					// set addressing mode
	sendCommand(HORIZONTAL_ADDRESSING); // set page addressing mode
}

// startscrollright
// Activate a right handed scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// scrollRight(0x00, 0x0F)  - start - stop
void OLEDI2C::scrollRight(byte start, byte end, byte speed)
{

	sendCommand(OzOLED_RIGHT_SCROLL); // Horizontal Scroll Setup
	sendCommand(0x00);				  // dummy byte
	sendCommand(start);				  // start page address
	sendCommand(speed);				  // set time interval between each scroll
	sendCommand(end);				  // end page address

	sendCommand(0x01);
	sendCommand(0xFF);

	sendCommand(0x2f); // active scrolling
}

// startscrollleft
// Activate a right handed scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F)   - start - stop
void OLEDI2C::scrollLeft(byte start, byte end, byte speed)
{

	sendCommand(OzOLED_LEFT_SCROLL); // Horizontal Scroll Setup
	sendCommand(0x00);				 // dummy byte
	sendCommand(start);				 // start page address
	sendCommand(speed);				 // set time interval between each scroll
	sendCommand(end);				 // end page address

	sendCommand(0x01);
	sendCommand(0xFF);

	sendCommand(0x2f); // active scrolling
}

// startscrolldiagright
// Activate a diagonal scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F)
void OLEDI2C::scrollDiagRight()
{

	sendCommand(OzOLED_SET_VERTICAL_SCROLL_AREA);
	sendCommand(0X00);
	sendCommand(OzOLED_Max_Y);
	sendCommand(OzOLED_VERTICAL_RIGHT_SCROLL); // Vertical and Horizontal Scroll Setup
	sendCommand(0X00);						   // dummy byte
	sendCommand(0x00);						   // define page0 as startpage address
	sendCommand(0X00);						   // set time interval between each scroll ste as 6 frames
	sendCommand(0x07);						   // define page7 as endpage address
	sendCommand(0X01);						   // set vertical scrolling offset as 1 row
	sendCommand(OzOLED_CMD_ACTIVATE_SCROLL);   // active scrolling
}

void OLEDI2C::scrollDiagLeft()
{

	sendCommand(OzOLED_SET_VERTICAL_SCROLL_AREA);
	sendCommand(0X00);
	sendCommand(OzOLED_Max_Y);
	sendCommand(OzOLED_VERTICAL_LEFT_SCROLL); // Vertical and Horizontal Scroll Setup
	sendCommand(0X00);						  // dummy byte
	sendCommand(0x00);						  // define page0 as startpage address
	sendCommand(0X00);						  // set time interval between each scroll ste as 6 frames
	sendCommand(0x07);						  // define page7 as endpage address
	sendCommand(0X01);						  // set vertical scrolling offset as 1 row
	sendCommand(OzOLED_CMD_ACTIVATE_SCROLL);  // active scrolling
}

void OLEDI2C::setActivateScroll(byte direction, byte startPage, byte endPage, byte scrollSpeed)
{

	/*
	This function is still not complete, we need more testing also.
	Use the following defines for 'direction' :

	 Scroll_Left
	 Scroll_Right

	For Scroll_vericle, still need to debug more...

	Use the following defines for 'scrollSpeed' :

	 Scroll_2Frames
	 Scroll_3Frames
	 Scroll_4Frames
	 Scroll_5Frames
	 Scroll_25Frames
	 Scroll_64Frames
	 Scroll_128Frames
	 Scroll_256Frames

	*/

	if (direction == Scroll_Right)
	{
		// Scroll Right
		sendCommand(0x26);
	}
	else
	{
		// Scroll Left
		sendCommand(0x27);
	}

	sendCommand(0x00); // dummy byte
	sendCommand(startPage);
	sendCommand(scrollSpeed);
	sendCommand(endPage); // for verticle scrolling, use 0x29 as command, endPage should = start page = 0

	sendCommand(OzOLED_CMD_ACTIVATE_SCROLL);
}

void OLEDI2C::setDeactivateScroll()
{
	sendCommand(OzOLED_CMD_DEACTIVATE_SCROLL);
}