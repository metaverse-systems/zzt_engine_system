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
    void BoardLoad(int16_t board);
    void BoardLoadText(int16_t board);
    void BoardChange(int16_t board);

    bool ElementMove(int16_t board_index, uint8_t el_id, char direction);
};
