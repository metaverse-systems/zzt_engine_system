#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>

#include "zztElements.hpp"
#include "zztOOP.hpp"

typedef struct
{
    int16_t BoardSize;
    uint8_t BoardNameLength;
    char BoardName[50];
} __attribute__((__packed__)) zztBoardHeader;

typedef struct
{
    uint8_t MaxPlayerShots;
    uint8_t IsDark;
    uint8_t ExitNorth;
    uint8_t ExitSouth;
    uint8_t ExitWest;
    uint8_t ExitEast;
    uint8_t RestartOnZap;
    uint8_t MessageLength;
    char    Message[58];
    uint8_t PlayerEnterX;
    uint8_t PlayerEnterY;
    int16_t TimeLimit;
    uint8_t unused[16];
    int16_t StatElementCount;
} __attribute__((__packed__)) zztBoardProperties;

typedef struct
{
    uint8_t Count;
    uint8_t Element;
    uint8_t Color;
} __attribute__((__packed__)) zztRLE;

class zztWorld;
class zztOOP;

class zztBoard
{
  public:
    zztBoard(zztWorld *world, int16_t board);
    int16_t SizeGet();
    void HeaderDump();
    void PropertiesDump();
    void StatusElementDump(zztStatusElement *);
    zztWorld *world;
    std::vector<uint16_t> board_elements;
    std::vector<zztStatusElement *> status_elements;
    std::vector<zztOOP *> status_elements_code;
    std::unordered_map<std::string, uint8_t> status_elements_names;
  private:
    uint8_t *data = nullptr;
    uint16_t rle_size = 0;
    zztBoardHeader *header = nullptr;
    zztBoardProperties board_properties;
    std::string name;
    void rleExpand();
    void StatusElementParse();
};
