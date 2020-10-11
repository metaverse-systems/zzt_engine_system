#include <map>
#include <string>
#include "zztColors.hpp"

std::map<std::string, zztColor> zztPalette;
std::map<uint8_t, std::string> zztColors;

void initZZTpalette()
{
    zztColors[0] = "black";
    zztPalette["black"].r = 0;             zztPalette["black"].g = 0;             zztPalette["black"].b = 0;

    zztColors[1] = "blue";
    zztPalette["blue"].r = 0;              zztPalette["blue"].g = 0;              zztPalette["blue"].b = 170;

    zztColors[2] = "green";
    zztPalette["green"].r = 0;             zztPalette["green"].g = 170;           zztPalette["green"].b = 0;

    zztColors[3] = "cyan";
    zztPalette["cyan"].r = 0;              zztPalette["cyan"].g = 170;            zztPalette["cyan"].b = 170;

    zztColors[4] = "red";
    zztPalette["red"].r = 170;             zztPalette["red"].g = 0;               zztPalette["red"].b = 0;

    zztColors[5] = "magenta";
    zztPalette["magenta"].r = 170;         zztPalette["magenta"].g = 0;           zztPalette["magenta"].b = 170;

    zztColors[6] = "brown";
    zztPalette["brown"].r = 170;           zztPalette["brown"].g = 85;            zztPalette["brown"].b = 0;

    zztColors[7] = "white";
    zztPalette["white"].r = 170;           zztPalette["white"].g = 170;           zztPalette["white"].b = 170;

    zztColors[8] = "gray";
    zztPalette["gray"].r = 85;             zztPalette["gray"].g = 85;             zztPalette["gray"].b = 85;
 
    zztColors[9] = "light blue";
    zztPalette["light blue"].r = 85;       zztPalette["light blue"].g = 85;       zztPalette["light blue"].b = 255;

    zztColors[10] = "light green";
    zztPalette["light green"].r = 85;      zztPalette["light green"].g = 255;     zztPalette["light green"].b = 64;

    zztColors[11] = "light cyan";
    zztPalette["light cyan"].r = 85;       zztPalette["light cyan"].g = 255;      zztPalette["light cyan"].b = 255;

    zztColors[12] = "light red";
    zztPalette["light red"].r = 255;       zztPalette["light red"].g = 85;        zztPalette["light red"].b = 85;

    zztColors[13] = "light magenta";
    zztPalette["light magenta"].r = 255;   zztPalette["light magenta"].g = 85;    zztPalette["light magenta"].b = 255;

    zztColors[14] = "yellow";
    zztPalette["yellow"].r = 255;          zztPalette["yellow"].g = 255;          zztPalette["yellow"].b = 85;

    zztColors[15] = "bright white";
    zztPalette["bright white"].r = 255;    zztPalette["bright white"].g = 255;    zztPalette["bright white"].b = 255;
}
