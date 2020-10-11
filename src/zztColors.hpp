#pragma once
#include <map>
#include <string>

typedef struct
{
        uint8_t r, g, b;
} zztColor;    

extern std::map<std::string, zztColor> zztPalette;
extern std::map<uint8_t, std::string> zztColors;

void initZZTpalette();
