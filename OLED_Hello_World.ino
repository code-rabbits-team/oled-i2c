#include "FontTable.h"
#include "src/libs/SlimList.h"
#include "src/libs/Helper.h"
#include "src/libs/PredefineFont.h"
#include <Arduino.h>
#include "oled-i2c.h"

FontTable table(3);
OLEDI2C oled(&table, 130, 64);

void setup()
{
  Serial.begin(115200);
  table.RegisterFontIndex(&AsciiFontIndex);

  oled.init();
  oled.setCursorXY(0, 0);
  oled.print("27.1℃");
}

void loop()
{
}
