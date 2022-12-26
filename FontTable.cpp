#include "FontTable.h"

void DebugPrint(SlimList<FontIndex_t *> *plist)
{
    Serial.println("[DebugPrint]");

    for (size_t i = 0; i < plist->getCount(); i++)
    {
        const FontIndex_t *pFontIndex = (*plist)[i];
        Serial.print("pFontIndex: ");
        Serial.print((int16_t)pFontIndex);
        Serial.print(", offset: ");
        Serial.print(pFontIndex->offset);
        Serial.print(",  length: ");
        Serial.print(pFontIndex->length);
        Serial.print(", count: ");
        Serial.print(plist->getCount());
        Serial.print(", i: ");
        Serial.print(i);
        Serial.println();
    }
}

uint32_t FontTable::Utf8Parser(const char *ch, size_t length)
{
    uint32_t font = 0;
    int cursor = -1;
    while (++cursor < length)
    {
        font = font << 8;
        font |= ((uint8_t *)ch)[cursor];
    }

    return font;
}

ssize_t FontTable::GetChar(const char *ch, uint32_t *fontIndex)
{
    ssize_t size = 1;

    if ((*ch & 0b11110000) == 0b11110000)
    {
        size = 4;
    }
    else if ((*ch & 0b11100000) == 0b11100000)
    {
        size = 3;
    }
    else if ((*ch & 0b11000000) == 0b11000000)
    {
        size = 2;
    }

    *fontIndex = Utf8Parser(ch, size);
    return size;
}

// @TODO
// If there are more than 3, it does not work properly,
// so please improve it and delete this comment when the improvement is completed.
ssize_t FontTable::BinarySearchIndex(uint32_t fontIndex)
{
    size_t length = _fontList.getCount();
    size_t cursor = length / 2;

    while (length > 0)
    {
        FontIndex_t *target = _fontList[cursor];
        if (target->offset <= fontIndex && fontIndex < target->offset + target->length)
        {
            return cursor;
        }

        if (fontIndex < target->offset)
        {
            cursor -= length / 2;
        }
        else
        {
            cursor += (length + 1) / 2;
        }

        length /= 2;
    }

    return cursor;
}

FontTable::FontTable() : FontTable::FontTable(0)
{
}

FontTable::FontTable(size_t capacity) : FontTable::FontTable(SlimList<FontIndex_t *>(capacity))
{
}

FontTable::FontTable(SlimList<FontIndex_t *> fontList) : _fontList(fontList)
{
}

bool FontTable::RegisterFontIndex(const FontIndex_t *fontIndex)
{
    size_t index = BinarySearchIndex(fontIndex->offset);
    return _fontList.Insert(index, fontIndex);
}

size_t FontTable::GetFont(const char *ch, const byte **font)
{
    uint32_t f = 0;
    size_t length = GetChar(ch, &f);

    size_t fIndex = BinarySearchIndex(f);
    FontIndex_t *fontIndex = _fontList[fIndex];

    const uint8_t *pFont PROGMEM = NULL;
    size_t targetIndex = f - fontIndex->offset;
    if (0 <= targetIndex && targetIndex < fontIndex->length)
    {
        pFont = (uint8_t *)(fontIndex->font + (targetIndex * 4));
    }

    for (byte i = 0; i < 8; i++)
    {
        byte b = pgm_read_byte(&(pFont[i]));
    }

    *font = pFont;
    return length;
}
