#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>

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

typedef struct
{
    uint8_t LocationX;
    uint8_t LocationY;
    int16_t StepX;
    int16_t StepY;
    int16_t Cycle;
    uint8_t P1;
    uint8_t P2;
    uint8_t P3;
    int16_t Follower;
    int16_t Leader;
    uint8_t UnderID;
    uint8_t UnderColor;
    int32_t Pointer;
    int16_t CurrentInstruction;
    int16_t Length;
    char Padding[8];
} __attribute__((__packed__)) zztStatusElement;

class zztOOP
{
  public:
    zztOOP(int16_t length, char *ptr);
  private:
    int16_t length;
    char *ptr = nullptr;
    std::string name;
    std::unordered_map<std::string, int16_t> labels;
    void Parse();
};

class zztBoard
{
  public:
    zztBoard(uint8_t *address);
    int16_t SizeGet();
    void HeaderDump();
    void PropertiesDump();
    void StatusElementDump();
  private:
    uint8_t *data = nullptr;
    uint16_t rle_size = 0;
    std::vector<uint16_t> board_elements;
    zztBoardHeader *header = nullptr;
    zztBoardProperties board_properties;
    std::string name;
    void rleExpand();
};
