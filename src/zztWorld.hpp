#pragma once

#include <string>
#include <vector>
#include "zztBoard.hpp"

typedef struct
{
    int16_t WorldType;
    int16_t NumBoards;
    int16_t PlayerAmmo;
    int16_t PlayerGems;
    char PlayerKeys[7];
    int16_t PlayerHealth;
    int16_t PlayerBoard;

    int16_t PlayerTorches;
    int16_t TorchCycles;
    int16_t EnergyCycles;
    int16_t unused_1;
    int16_t PlayerScore;
    uint8_t WorldNameLength;
    char WorldName[20];

    uint8_t Flag0Length;
    char Flag0[20];
    uint8_t Flag1Length;
    char Flag1[20];
    uint8_t Flag2Length;
    char Flag2[20];
    uint8_t Flag3Length;
    char Flag3[20];
    uint8_t Flag4Length;
    char Flag4[20];
    uint8_t Flag5Length;
    char Flag5[20];
    uint8_t Flag6Length;
    char Flag6[20];
    uint8_t Flag7Length;
    char Flag7[20];
    uint8_t Flag8Length;
    char Flag8[20];
    uint8_t Flag9Length;
    char Flag9[20];
    uint8_t FlagXLength;
    char FlagX[20];

    int16_t TimePassed;
    int16_t TimePassedTicks;
    uint8_t Locked;
    uint8_t unused_2[14];

} __attribute__((__packed__)) zztWorldHeader;

class zztWorld
{
  public:
    zztWorld(std::string filename);
    ~zztWorld();
    void HeaderDump();
    zztBoard *BoardGet(int16_t board);
    uint8_t *data = nullptr;
    zztWorldHeader HeaderGet();
    void BoardChange(int16_t board);
  private:
    std::string filename;
    std::vector<std::string> flags;
};
