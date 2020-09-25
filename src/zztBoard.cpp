#include "zztBoard.hpp"
#include "zztWorld.hpp"
#include <iostream>
#include <algorithm>


const std::vector<std::string> explode(const std::string &s, const char &c)
{
    std::string buff{""};
    std::vector<std::string> v;

    for(auto n : s)
    {
        if(n != c) buff += n;
        else
        {
            if(n == c && buff != "")
            {
                v.push_back(buff);
                buff = ""; 
            }
        }
    }
    if(buff != "") v.push_back(buff);

    return v;
}

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
            new zztOOP(this->world, ptr);
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

//            for(const auto &line : lines) std::cout << line << std::endl;
        }
        ptr += sizeof(zztStatusElement);
        if(element->Length > 0) ptr += element->Length;
    }
}

zztOOP::zztOOP(zztWorld *world, uint8_t *ptr)
{
    this->world = world;
    this->element = (zztStatusElement *)ptr;
    this->code = ptr + sizeof(zztStatusElement);
    this->Parse();
    std::cout << "Name: " << this->name << std::endl;
    std::cout << "Length: " << this->element->Length << std::endl;
    while(this->Step());
    std::cout << "OOP script ended." << std::endl;
    this->Jump(":bribe");
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
                case '@': // name
                    this->name = temp;
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
}

bool zztOOP::Step()
{
    std::string temp = "";
    bool result = true;
    while(this->element->CurrentInstruction < this->element->Length)
    {
        if(this->code[this->element->CurrentInstruction] == '\r')
        {
//            std::cout << "Processing " << temp << std::endl;
            std::string command;
            std::vector<std::string> tokens;
            switch(temp[0])
            {
                case '#': // Command
                    command = temp;
                    std::transform(command.begin(), command.end(), command.begin(), ::tolower);
                    tokens = explode(command, ' ');
                    if(tokens[0] == "#end")
                    {
                        result = false;
                    }

                    if(tokens[0] == "#lock")
                    {
                        this->locked = true;
                    }

                    if(tokens[0] == "#unlock")
                    {
                        this->locked = false;
                    }

                    if(tokens[0] == "#restart")
                    {
                        this->element->CurrentInstruction = 0;
                        return true;
                    }

                    if(tokens[0] == "#idle")
                    {

                    }

                    if(tokens[0] == "#endgame")
                    {
                    }

                    if(tokens[0] == "#die")
                    {
                    }

                    if(tokens[0] == "#take")
                    {
                        if(!this->Take(tokens[1], std::stoi(tokens[2])))
                        {
                            if(tokens.size() == 4)
                            {
                                this->element->CurrentInstruction = this->labels[tokens[3]];
                                return true;
                            }
                        }
                    }
                default:
                    int16_t addr = this->element->CurrentInstruction - temp.size();
                    std::cout << std::to_string(addr) << "| " << temp << std::endl;
                    break;
            }
            this->element->CurrentInstruction++;
            return result;
        }
        else temp += this->code[this->element->CurrentInstruction];
        this->element->CurrentInstruction++;
    }
    result = false;
    return result;
}

void zztOOP::Jump(std::string label)
{
    if(this->locked) return;

    int16_t jmp = this->labels[label];
    this->element->CurrentInstruction = jmp;
}

bool zztOOP::Take(std::string item, int16_t qty)
{
    zztWorldHeader *header = (zztWorldHeader *)this->world->data;
    int16_t *value = nullptr;
    if(item == "ammo") value = &header->PlayerAmmo;
    if(item == "torches") value = &header->PlayerTorches;
    if(item == "gems") value = &header->PlayerGems;
    if(item == "health") value = &header->PlayerHealth;
    if(item == "score") value = &header->PlayerScore;
//    if(item == "time") value = &header->Player

    if(value == nullptr) return false;
    if(qty > *value) return false;
    *value -= qty;
    return true;
}

void zztOOP::Give(std::string item, int16_t qty)
{
}
