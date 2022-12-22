#ifndef FONT_TABLE_H_
#define FONT_TABLE_H_

#include "src/libs/SlimList.h";
#include "src/libs/FontType.h";


class FontList : public SlimList<FontIndex_t> { };


class FontTable
{
private:
    FontList _fontList;

public:
    FontTable();

    int getCount();
};

#endif