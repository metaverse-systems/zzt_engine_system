#include "zztBoard.hpp"
#include "zztWorld.hpp"
#include <iostream>
#include <algorithm>

zztBoard::zztBoard(zztWorld *world, int16_t board)
{
    int16_t counter = 0;
    uint8_t *address = world->data + 0x200; // Assuming ZZT
    zztWorldHeader *header = (zztWorldHeader *)world->data;

    if(board >= header->NumBoards) throw std::runtime_error("Board " + std::to_string(board) + " not found.");

    do
    {
        if(board == counter)
        {
            break;
        }

        int16_t BoardSize = *((int16_t *)(address));
        address += BoardSize + 2;

        counter++;
    } while(counter < header->NumBoards);

    this->world = world;
    this->data = address;
    this->header = (zztBoardHeader *)this->data;
    this->rleExpand();
    this->board_properties = *(zztBoardProperties *)(this->data + this->rle_size + 0x35);
    this->StatusElementParse();
}

int16_t zztBoard::SizeGet()
{
    return this->header->BoardSize;
}

void zztBoard::HeaderDump()
{
    std::cout << "Board size: " << this->header->BoardSize << " bytes" << std::endl;
        
    this->name = std::string(this->header->BoardName).substr(0, this->header->BoardNameLength);
    std::cout << "Board name: " << this->name << std::endl;
}

void zztBoard::PropertiesDump()
{
    std::cout << "MaxPlayerShots: " << static_cast<unsigned int>(this->board_properties.MaxPlayerShots) << std::endl;
    std::cout << "IsDark: " << (this->board_properties.IsDark ? "Yes" : "No") << std::endl;
    std::cout << "North board: " << static_cast<unsigned int>(this->board_properties.ExitNorth) << std::endl;
    std::cout << "South board: " << static_cast<unsigned int>(this->board_properties.ExitSouth) << std::endl;
    std::cout << "West board: " << static_cast<unsigned int>(this->board_properties.ExitWest) << std::endl;
    std::cout << "East board: " << static_cast<unsigned int>(this->board_properties.ExitEast) << std::endl;
    std::cout << "RestartOnZap: " << (this->board_properties.RestartOnZap ? "Yes" : "No") << std::endl;
    std::cout << "MessageLength: " << static_cast<unsigned int>(this->board_properties.MessageLength) << std::endl;
    std::cout << "Message: " << this->board_properties.Message << std::endl;
    std::cout << "PlayerEnterX: " << static_cast<unsigned int>(this->board_properties.PlayerEnterX) << std::endl;
    std::cout << "PlayerEnterY: " << static_cast<unsigned int>(this->board_properties.PlayerEnterY) << std::endl;
    std::cout << "TimeLimit: " << static_cast<unsigned int>(this->board_properties.TimeLimit) << " seconds" << std::endl;
    std::cout << "StatElementCount: " << static_cast<unsigned int>(this->board_properties.StatElementCount) << std::endl;
}

void zztBoard::rleExpand()
{
    uint16_t tiles = 0;
    zztRLE *rle = (zztRLE *)(this->data + 0x35);

    this->rle_size = 0;
    while(tiles < 1500)
    {
        uint16_t counter = 0;
        while(counter < rle->Count)
        {
            this->board_elements.push_back((rle->Color << 8) | rle->Element);
            counter++; tiles++;
        }
        rle++;
        this->rle_size += 3;
    }
}

void zztBoard::StatusElementParse()
{
    this->status_elements.resize(this->board_properties.StatElementCount + 1);
    this->status_elements_code.resize(this->board_properties.StatElementCount + 1);

    uint8_t *ptr = this->data + this->rle_size + 0x35 + sizeof(zztBoardProperties);
    for(uint8_t i = 0; i <= this->board_properties.StatElementCount; i++)
    {
        this->status_elements[i] = (zztStatusElement *)ptr;
        if(this->status_elements[i]->Length > 0)
        {
            this->status_elements_code[i] = new zztOOP(this->world, this, i);
            ptr += this->status_elements[i]->Length;
        }

        ptr += sizeof(zztStatusElement);
    }
}

void zztBoard::StatusElementDump(zztStatusElement *element)
{
    std::cout << "LocationX: " << static_cast<unsigned int>(element->LocationX) << std::endl;
    std::cout << "LocationY: " << static_cast<unsigned int>(element->LocationY) << std::endl;
    std::cout << "StepX: " << static_cast<int>(element->StepX) << std::endl;
    std::cout << "StepY: " << static_cast<int>(element->StepY) << std::endl;
    std::cout << "Cycle: " << static_cast<int>(element->Cycle) << std::endl;
    std::cout << "P1: " << static_cast<unsigned int>(element->P1) << std::endl;
    std::cout << "P2: " << static_cast<unsigned int>(element->P2) << std::endl;
    std::cout << "P3: " << static_cast<unsigned int>(element->P3) << std::endl;
    std::cout << "Follower: " << std::hex << static_cast<int>(element->Follower) << std::endl;
    std::cout << "Leader: " << std::hex << static_cast<int>(element->Leader) << std::endl;
    std::cout << "UnderID: " << static_cast<unsigned int>(element->UnderID) << std::endl;
    std::cout << "UnderColor: " << static_cast<unsigned int>(element->UnderColor) << std::endl;
    std::cout << "Pointer: " << static_cast<int>(element->Pointer) << std::endl;
    std::cout << "CurrentInstruction: " << static_cast<int>(element->CurrentInstruction) << std::endl;
//    std::cout << "Padding: " << element->Padding << std::endl;
}
