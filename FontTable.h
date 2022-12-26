#ifndef FONT_TABLE_H_
#define FONT_TABLE_H_

#include "src/libs/SlimList.h";
#include "src/libs/FontType.h";

class FontTable
{
private:
    SlimList<FontIndex_t *> _fontList;

    uint32_t Utf8Parser(const char *ch, size_t length);
    ssize_t GetChar(const char *ch, uint32_t *fontIndex);
    ssize_t BinarySearchIndex(uint32_t fontIndex);

public:
    FontTable();
    FontTable(size_t capacity);
    FontTable(SlimList<FontIndex_t *> fontList);

    bool RegisterFontIndex(const FontIndex_t *fontIndex);
    size_t GetFont(const char *ch, const byte **font);
};

#endif