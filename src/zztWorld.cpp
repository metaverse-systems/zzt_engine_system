#include "zztWorld.hpp"
#include <fstream>
#include <iostream>

zztWorld::zztWorld(std::string filename)
{
    this->flags.resize(10);
    this->filename = filename;

    std::fstream file;
    file.open(filename.c_str(), std::fstream::in);

    if(!file.good())
    {
        throw std::runtime_error("Couldn't open file " +  filename);
    }

    file.seekg(0, std::ios::end);
    size_t sz = file.tellg();

    this->data = new uint8_t[sz];
    file.seekg(0, std::ios::beg);
    file.read((char *)this->data, sz);
    file.close();
}

zztWorld::~zztWorld()
{
    delete this->data;
}

void zztWorld::HeaderDump()
{
    int16_t file_type = *((int16_t *)this->data);
    std::string msg, name;

    zztWorldHeader *header;

    switch(file_type)
    {
        case -1: // ZZT
            header = (zztWorldHeader *)this->data;
            name = std::string(header->WorldName).substr(0, header->WorldNameLength);

            std::cout << "Type: ZZT" << std::endl;
            std::cout << "Number of boards: " << header->NumBoards << std::endl;
            std::cout << "Player ammo: " << header->PlayerAmmo << std::endl;
            std::cout << "Player gems: " << header->PlayerGems << std::endl;

            std::cout << "Player keys: " << std::endl;
            std::cout << "Blue - " << (header->PlayerKeys[0] ? "Yes" : "No") << std::endl;
            std::cout << "Green - " << (header->PlayerKeys[1] ? "Yes" : "No") << std::endl;
            std::cout << "Cyan - " << (header->PlayerKeys[2] ? "Yes" : "No") << std::endl;
            std::cout << "Red - " << (header->PlayerKeys[3] ? "Yes" : "No") << std::endl;
            std::cout << "Purple - " << (header->PlayerKeys[4] ? "Yes" : "No") << std::endl;
            std::cout << "Yellow - " << (header->PlayerKeys[5] ? "Yes" : "No") << std::endl;
            std::cout << "White - " << (header->PlayerKeys[6] ? "Yes" : "No") << std::endl;

            std::cout << "Player health: " << header->PlayerHealth << std::endl;
            std::cout << "Player board: " << header->PlayerBoard << std::endl;
            std::cout << "Player torches: " << header->PlayerTorches << std::endl;
            std::cout << "Torch cycles: " << header->TorchCycles << std::endl;
            std::cout << "Energy cycles: " << header->EnergyCycles << std::endl;
            std::cout << "Player score: " << header->PlayerScore << std::endl;

            std::cout << "World name: " << name << std::endl;

            this->flags[0] = std::string(header->Flag0).substr(0, header->Flag0Length);
            this->flags[1] = std::string(header->Flag1).substr(0, header->Flag1Length);
            this->flags[2] = std::string(header->Flag2).substr(0, header->Flag2Length);
            this->flags[3] = std::string(header->Flag3).substr(0, header->Flag3Length);
            this->flags[4] = std::string(header->Flag4).substr(0, header->Flag4Length);
            this->flags[5] = std::string(header->Flag5).substr(0, header->Flag5Length);
            this->flags[6] = std::string(header->Flag6).substr(0, header->Flag6Length);
            this->flags[7] = std::string(header->Flag7).substr(0, header->Flag7Length);
            this->flags[8] = std::string(header->Flag8).substr(0, header->Flag8Length);
            this->flags[9] = std::string(header->Flag9).substr(0, header->Flag9Length);

            for(auto &f : this->flags)
            {
                std::cout << "Flag: " << f << std::endl;
            }

            std::cout << "Time passed: " << header->TimePassed << std::endl;
            std::cout << "Time passed (ticks): " << header->TimePassedTicks << std::endl;

            if(header->Locked) std::cout << "This is a saved game." << std::endl;
            else std::cout << "This is a game world." << std::endl;

            break;
        case -2: // SuperZZT
            throw std::runtime_error("SuperZZT not supported.");
        break;
        default:
            throw std::runtime_error("Unknown file type" + file_type);
            break;
    }    
}

zztBoard *zztWorld::BoardGet(int16_t board)
{
    return new zztBoard(this, board);
}
