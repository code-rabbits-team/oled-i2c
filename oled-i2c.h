/*
  OzOLED.h - 0.96' I2C 128x64 OLED Driver Library
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
#ifndef OLED_I2C_H_
#define OLED_I2C_H_

#include <Arduino.h>
#include <Wire.h>
#include "FontTable.h"

#define OzOLED_Max_X 128 // 128 Pixels
#define OzOLED_Max_Y 64	 // 64  Pixels

#define OLED_ADDRESS 0x3C
#define I2C_400KHZ 1 // 0 to use default 100Khz, 1 for 400Khz

// registers
#define OzOLED_COMMAND_MODE 0x80
#define OzOLED_DATA_MODE 0x40

// commands
#define OzOLED_CMD_DISPLAY_OFF 0xAE
#define OzOLED_CMD_DISPLAY_ON 0xAF
#define OzOLED_CMD_NORMAL_DISPLAY 0xA6
#define OzOLED_CMD_INVERSE_DISPLAY 0xA7
#define OzOLED_CMD_SET_BRIGHTNESS 0x81

#define OzOLED_RIGHT_SCROLL 0x26
#define OzOLED_LEFT_SCROLL 0x27
#define OzOLED_SET_VERTICAL_SCROLL_AREA 0xA3
#define OzOLED_VERTICAL_RIGHT_SCROLL 0x29
#define OzOLED_VERTICAL_LEFT_SCROLL 0x2A
#define OzOLED_CMD_ACTIVATE_SCROLL 0x2F
#define OzOLED_CMD_DEACTIVATE_SCROLL 0x2E

#define HORIZONTAL_ADDRESSING 0x00
#define PAGE_ADDRESSING 0x02

#define Scroll_Left 0x00
#define Scroll_Right 0x01
#define Scroll_Up 0x02
#define Scroll_Down 0x03

#define Scroll_2Frames 0x07
#define Scroll_3Frames 0x04
#define Scroll_4Frames 0x05
#define Scroll_5Frames 0x00
#define Scroll_25Frames 0x06
#define Scroll_64Frames 0x01
#define Scroll_128Frames 0x02
#define Scroll_256Frames 0x03

/// @brief Control OLED through I2C.
class OLEDI2C
{

private:
	FontTable *_fontTable;
	TwoWire _wire;
	uint8_t _width;
	uint8_t _height;
	uint8_t _cursorX;
	uint8_t _cursorY;

	void print(byte **font);

public:
	/// @brief Initializes a new instance of the OLEDI2C class with default communication and size.
	/// @param fontTable The table with fonts to display on the oled.
	OLEDI2C(FontTable *fontTable);

	/// @brief Initializes a new instance of the OLEDI2C class with default communication and specified size
	/// @param fontTable The table with fonts to display on the oled.
	/// @param height The width of the height.
	/// @param width The width of the oled.
	OLEDI2C(FontTable *fontTable, uint8_t width, uint8_t height);

	/// @brief Initializes a new instance of the OLEDI2C class with specified communication and size
	/// @param wire The wire is the TwoWire instance used to communicate with the oled device..
	/// @param fontTable The table with fonts to display on the oled.
	/// @param height The width of the height.
	/// @param width The width of the oled.
	OLEDI2C(TwoWire wire, FontTable *fontTable, uint8_t width, uint8_t height);

	byte addressingMode;

	void sendCommand(byte command);
	void sendData(byte Data);

	size_t print(const char *str);
	size_t print(const char *str, size_t length);
	byte printNumber(long n, byte X = 255, byte Y = 255);
	byte printNumber(float float_num, byte prec = 6, byte Y = 255, byte numChar = 255);
	void printBigNumber(const char *number, byte column = 0, byte page = 0, byte numChar = 255);
	void drawBitmap(const byte *bitmaparray, byte X, byte Y, byte width, byte height);

	void init();

	void setCursorXY(byte x, byte y);
	void clearDisplay();
	// void clearPage(byte page);

	void setNormalDisplay();
	void setInverseDisplay();
	void setPowerOff();
	void setPowerOn();
	void setPageMode();
	void setHorizontalMode();
	void setBrightness(byte Brightness);

	void scrollRight(byte start, byte end, byte speed);
	void scrollLeft(byte start, byte end, byte speed);
	void scrollDiagRight();
	void scrollDiagLeft();
	void setActivateScroll(byte direction, byte startPage, byte endPage, byte scrollSpeed);
	void setDeactivateScroll();
};

#endif
