#pragma once
#include <string>
#include <vector>

extern std::vector<uint8_t> zztElements;

void initZZTelements();

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
