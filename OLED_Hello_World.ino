#include <Wire.h>
#include "oled-i2c.h"
#include "FontTable.h"
#include "src/libs/SlimList.h"
#include "src/libs/Helper.h"
#include <Arduino.h>

// OLEDI2C oled(130, 64);
FontTable table;

void setup()
{
  Serial.begin(9600);

  Serial.println(table.getCount());
}

void loop()
{
}
