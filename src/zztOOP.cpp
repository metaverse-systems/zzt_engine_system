#include "zztOOP.hpp"
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

void zztLabel::Add(int16_t addr)
{
    this->addresses.push_back(addr);
}

int16_t zztLabel::Get()
{
    if(this->index >= this->addresses.size()) return -1;
    return this->addresses[this->index];
}

void zztLabel::Zap()
{
    this->index++;
}

void zztLabel::Restore()
{
    if(this->index == 0) return;
    this->index--;
}

zztOOP::zztOOP(zztWorld *world, zztBoard *board, uint8_t el_number)
{
    this->world = world;
    this->board = board;
    this->el_number = el_number;
    this->length = this->board->status_elements[this->el_number]->Length;
    this->element = this->board->status_elements[this->el_number];
    this->code = (uint8_t *)this->board->status_elements[this->el_number] + sizeof(zztStatusElement);
    this->Parse();
}

void zztOOP::Dump()
{
    std::string temp;
    std::vector<std::string> lines;
    for(uint16_t counter = 0; counter < this->length; counter++)
    {
        if(this->code[counter] == '\r')
        {
            lines.push_back(temp);
            temp = "";
        }
        else temp += this->code[counter];
    }

    for(const auto &line : lines) std::cout << line << std::endl;
}

void zztOOP::Parse()
{
    std::string temp = "";
    for(int16_t counter = 0; counter < this->length; counter++)
    {
        if(this->code[counter] == '\r')
        {
            switch(temp[0])
            {
                case '@': // name
                    this->name = temp;
                    this->board->status_elements_names[this->name].push_back(this->el_number);
                    break;
                case ':': // label
                    this->labels[temp].Add(counter - temp.size());
                    break;
                case '#': // command
                    break;
                default:
                    break;
            }
            temp = "";
        }
        else temp += this->code[counter];
    }
}

bool zztOOP::Step(std::shared_ptr<zzt_status_el_component> z)
{
    if(z->running == false) return false;

    auto element = this->board->status_elements[z->id];
    std::string temp = "";
    bool result = true;
    while(element->CurrentInstruction < element->Length)
    {
        if(this->code[element->CurrentInstruction] == '\r')
        {
            std::string command;
            std::vector<std::string> tokens;
            switch(temp[0])
            {
                case '#': // Command
                    command = temp;

                    // Convert to lowercase
                    std::transform(command.begin(), command.end(), command.begin(), ::tolower);

                    tokens = explode(command, ' ');
                    if(tokens[0] == "#end")
                    {
                        z->running = false;
                    }

                    if(tokens[0] == "#lock")
                    {
                        z->locked = true;
                    }

                    if(tokens[0] == "#unlock")
                    {
                        z->locked = false;
                    }

                    if(tokens[0] == "#restart")
                    {
                        element->CurrentInstruction = 0;
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
                                uint16_t next_label = this->labels[tokens[3]].Get();
                                if(next_label == -1) return true;
                                element->CurrentInstruction = next_label;
                                return true;
                            }
                        }
                    }

                    if(tokens[0] == "#bind")
                    {
                        this->Bind(tokens[1]);
                    }
                default:
                    int16_t addr = element->CurrentInstruction - temp.size();
                    std::cout << std::to_string(addr) << "| " << temp << std::endl;
                    break;
            }
            element->CurrentInstruction++;
            return result;
        }
        else temp += this->code[element->CurrentInstruction];
        element->CurrentInstruction++;
    }
    result = false;
    return result;
}

void zztOOP::Jump(std::string label)
{
    auto element = this->element;
    if(this->locked) return;

    int16_t jmp = this->labels[label].Get();
    if(jmp == -1) return;
    element->CurrentInstruction = jmp;
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

void zztOOP::Bind(std::string name)
{
    auto element = this->element;

    auto index = this->board->status_elements_names["@" + name][0];
    this->board->status_elements_code[index]->Dump();

    element->CurrentInstruction = 0;
    this->board->status_elements_code[this->el_number] = this->board->status_elements_code[index];
    delete this;
}
