#include <libthe-seed/ComponentLoader.hpp>
#include <zzt_engine_system.hpp>
#include "zztColors.hpp"
#include "zztElements.hpp"
#include "zztOOP.hpp"
#include "../node_modules/@metaverse-systems/position_component/src/position_component.hpp"
#include "../node_modules/@metaverse-systems/text_component/src/text_component.hpp"
#include "../node_modules/@metaverse-systems/zzt_status_el_component/src/zzt_status_el_component.hpp"
#include <iostream>

zzt_engine_system::zzt_engine_system() 
{ 
    this->Handle = "zzt_engine_system";
}

zzt_engine_system::zzt_engine_system(Json::Value config)
{
    this->Handle = "zzt_engine_system";
    this->filename = config["filename"].asString();
    this->current_board = config["board"].asUInt();
}

Json::Value zzt_engine_system::Export()
{
    Json::Value config;
    config["filename"] = this->filename;
    config["board"] = this->current_board;
    return config;
}

void zzt_engine_system::Init()
{
    this->ComponentRequest("position_component");
    this->ComponentRequest("text_component");
    this->ComponentRequest("zzt_status_el_component");
    initZZTpalette();
    initZZTelements();
}

void zzt_engine_system::Update()
{
    auto dt = this->DeltaTimeGet();

    this->ms += dt;

    // 110ms == 9.1Hz
    if(this->ms < 110) return;
    this->ms -= 110;
    this->cycle++;

    if(this->world == nullptr)
    {
        if(this->filename.size() == 0) return;

        this->world = new zztWorld(filename);
        this->world->HeaderDump();
        return;
    }

    zztWorldHeader wheader = this->world->HeaderGet();
    if(this->boards.size() < wheader.NumBoards + 1) this->boards.resize(wheader.NumBoards + 1);    

    if(this->boards[this->current_board] == nullptr)
    {
        this->BoardLoad(this->current_board);
        return;
    }

    auto board = this->boards[this->current_board];
    auto Components = this->ComponentsGet();

    for(auto &[entity, zcomponent] : Components["zzt_status_el_component"])
    {
        auto z = std::dynamic_pointer_cast<zzt_status_el_component>(zcomponent);
        auto element = board->status_elements[z->id];

        if(element->Cycle) if((this->cycle % element->Cycle) != 0) continue;

        auto code = board->status_elements_code[z->id];
        if(code == nullptr) continue;
        board->status_elements_code[z->id]->Step(z);
    }
}

void zzt_engine_system::BoardLoad(uint8_t index)
{
    auto board = this->world->BoardGet(index);
    this->boards[index] = board;
    board->HeaderDump();
    board->PropertiesDump();
//  board->StatusElementDump();

    uint8_t row = 0, col = 0;
    for(auto &element : board->board_elements)
    {
        auto e = this->Container->Entity();

        auto pcomponent = e->Component(ComponentLoader::Create("@metaverse-systems/position_component"));
        auto tcomponent = e->Component(ComponentLoader::Create("@metaverse-systems/text_component"));

        auto p = std::dynamic_pointer_cast<position_component>(pcomponent);
        auto t = std::dynamic_pointer_cast<text_component>(tcomponent);

        p->x = col;
        p->y = row;

        auto fg_index = zztColors[(element >> 8) & 0xF];
        auto fg = zztPalette[fg_index];
        t->f_r = fg.r;
        t->f_g = fg.g;
        t->f_b = fg.b;

        auto bg_index = zztColors[(element >> 12) & 0x8];
        auto bg = zztPalette[bg_index];
        t->b_r = bg.r;
        t->b_g = bg.g;
        t->b_b = bg.b;
        t->c = zztElements[element & 0xFF];
        if(t->c == 0)
        {
            std::cout << "No element at index: " << (unsigned)(element & 0xFF) << std::endl;
        }

        col++;
        if(col > 59)
        {
            col = 0;
            row++;
        }
    }

    for(size_t i = 0; i < board->status_elements.size(); i++)
    {
        auto element = board->status_elements[i];
        auto Components = this->ComponentsGet();
        for(auto &[entity, pcomponent] : Components["position_component"])
        {
            auto p = std::dynamic_pointer_cast<position_component>(pcomponent);
            if((p->x != (element->LocationX - 1)) || (p->y != (element->LocationY - 1))) continue;

            auto e = this->Container->Entity(entity);
            auto zcomponent = e->Component(ComponentLoader::Create("@metaverse-systems/zzt_status_el_component"));
            auto z = std::dynamic_pointer_cast<zzt_status_el_component>(zcomponent);
            z->id = i;
        }
        auto code = board->status_elements_code[i];
        if(code != nullptr) code->Dump();
    }
}

extern "C"
{
    ecs::System *create_system(void *p)
    {
        if(p == nullptr) return new zzt_engine_system();

        Json::Value *config = (Json::Value *)p;
        return new zzt_engine_system(*config);
    }
}
