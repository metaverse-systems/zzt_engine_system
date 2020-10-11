#pragma once

#include <libecs-cpp/ecs.hpp>
#include "zztWorld.hpp"

class zzt_engine_system : public ecs::System
{
  public:
    zzt_engine_system(); 
    zzt_engine_system(Json::Value);
    Json::Value Export();
    void Update();
    void Init();
  private:
    uint16_t cycle = 0;
    uint16_t ms = 0;
    std::string filename;
    zztWorld *world = nullptr;
    std::vector<zztBoard *> boards;
    uint8_t current_board = 0;
    void BoardLoad(uint8_t board);
};
