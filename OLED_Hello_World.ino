#include "src/libs/FontTable.h"
#include "src/libs/SlimList.h"
#include "src/libs/Helper.h"
#include "src/libs/PredefineFont.h"
#include <Arduino.h>
#include "src/libs/oled-i2c.h"
#include "CustomFont.h"

FontTable table(3);
FontTable smallTable(3);
OLEDI2C oled(&table, 130, 64);

void setup()
{
  Serial.begin(115200);

  table.RegisterFontIndex(&AsciiFontIndex);
  table.RegisterFontIndex(&pFontIndex);

  smallTable.RegisterFontIndex(&SmallAsciiFontIndex);

  oled.init();
  oled.setCursorXY(0, 0);
  oled.print("123.356℃");
  oled.setCursorXY(0, 3);
  oled.setFontTable(&smallTable);
  oled.print("#$%\'()*+,");
  oled.setCursorXY(0, 4);
  oled.print("456789:<=>?@AB");
  oled.setCursorXY(0, 5);
  oled.print("0123456789");
}

void loop()
{
}
