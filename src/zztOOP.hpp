#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>

#include "zztElements.hpp"
#include "../node_modules/@metaverse-systems/zzt_status_el_component/src/zzt_status_el_component.hpp"

class zztWorld;
class zztBoard;

class zztOOP
{
  public:
    zztOOP(zztWorld *world, zztBoard *board, uint8_t el_number);
    void Jump(std::string label);
    void Dump();
    std::string name;
    uint8_t el_number;
    bool Step(std::shared_ptr<zzt_status_el_component>);
  private:
    zztWorld *world = nullptr;
    zztBoard *board = nullptr;
    int16_t length;
    zztStatusElement *element = nullptr;
    uint8_t *code = nullptr;
    std::unordered_map<std::string, int16_t> labels;
    bool locked = false;
    void Parse();
    bool Take(std::string item, int16_t qty);
    void Give(std::string item, int16_t qty);
    void Bind(std::string name);
};
