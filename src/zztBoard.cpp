#include "zztBoard.hpp"
#include <iostream>

zztBoard::zztBoard(uint8_t *address)
{
    this->data = address;
    this->header = (zztBoardHeader *)this->data;
    this->rleExpand();
    this->board_properties = *(zztBoardProperties *)(this->data + this->rle_size + 0x35);
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

void zztBoard::StatusElementDump()
{
    uint8_t *ptr = this->data + this->rle_size + 0x35 + sizeof(zztBoardProperties);
    for(uint8_t i = 0; i <= this->board_properties.StatElementCount; i++)
    {
        zztStatusElement *element = (zztStatusElement *)ptr;
        char *Code = (char *)(ptr + sizeof(zztStatusElement));
        std::cout << "LocationX: " << static_cast<unsigned int>(element->LocationX) << std::endl;
        std::cout << "LocationY: " << static_cast<unsigned int>(element->LocationY) << std::endl;
//        std::cout << "StepX: " << static_cast<int>(element->StepX) << std::endl;
//        std::cout << "StepY: " << static_cast<int>(element->StepY) << std::endl;
//        std::cout << "Cycle: " << static_cast<int>(element->Cycle) << std::endl;

        if(element->Length > 0)
        {
            new zztOOP(ptr);
            std::string temp;
            std::vector<std::string> lines;
            for(uint16_t counter = 0; counter < element->Length; counter++)
            {
                if(Code[counter] == '\r')
                {
                    lines.push_back(temp);
                    temp = "";
                }
                else temp += Code[counter];
            }

            for(const auto &line : lines) std::cout << line << std::endl;
        }
        ptr += sizeof(zztStatusElement);
        if(element->Length > 0) ptr += element->Length;
    }
}

zztOOP::zztOOP(uint8_t *ptr)
{
    this->element = (zztStatusElement *)ptr;
    this->code = ptr + sizeof(zztStatusElement);
    this->Parse();
    std::cout << "Name: " << this->name << std::endl;
    std::cout << "Length: " << this->element->Length << std::endl;
}

void zztOOP::Parse()
{
    std::string temp = "";
    for(int16_t counter = 0; counter < this->element->Length; counter++)
    {
        if(this->code[counter] == '\r')
        {
            switch(temp[0])
            {
                case '@':
                    this->name = temp;
                    break;
                case '#': // command
                    // if(temp == "#end")
                    break;
                case '\'': // comment
                    break;
                case ':': // label
                    this->labels[temp] = counter - temp.size();
                    break;
                default:
                    break;
            }
            temp = "";
        }
        else temp += this->code[counter];
    }

    for(const auto &label : this->labels)
    {
        std::cout << label.first << ": " << std::to_string(label.second) << std::endl;
    }
}
