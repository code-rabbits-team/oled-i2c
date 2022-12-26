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
  table.RegisterFontIndex(&koeraIndex1);
  Serial.println((uint16_t)(asciiFont));
  Serial.println((uint16_t)(asciiFont + 1));
  Serial.println((uint16_t)(asciiFont + 2));
  Serial.println((uint16_t)(asciiFont + 3));

  const char p[] = "Hello world!";
  oled.init();
  oled.setCursorXY(0, 0);
  oled.print("Hello world!");
  oled.setCursorXY(0, 1);
  oled.print(" 안");
}

void loop()
{
}
