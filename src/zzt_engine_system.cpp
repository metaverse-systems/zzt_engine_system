#include <libthe-seed/ComponentLoader.hpp>
#include <zzt_engine_system.hpp>
#include "zztColors.hpp"
#include "zztElements.hpp"
#include "zztOOP.hpp"
#include "../node_modules/@metaverse-systems/position_component/src/position_component.hpp"
#include "../node_modules/@metaverse-systems/text_component/src/text_component.hpp"
#include "../node_modules/@metaverse-systems/zzt_status_el_component/src/zzt_status_el_component.hpp"
#include "../node_modules/@metaverse-systems/input_component/src/input_component.hpp"
#include <iostream>

constexpr int32_t KEY_LEFT = 0x40000050;
constexpr int32_t KEY_RIGHT = 0x4000004F;
constexpr int32_t KEY_UP = 0x40000052;
constexpr int32_t KEY_DOWN = 0x40000051;

zzt_engine_system::zzt_engine_system() 
{ 
    this->Handle = "zzt_engine_system";
}

zzt_engine_system::zzt_engine_system(Json::Value config)
{
    this->Handle = "zzt_engine_system";
    this->filename = config["filename"].asString();
}

Json::Value zzt_engine_system::Export()
{
    Json::Value config;
    config["filename"] = this->filename;
    return config;
}

void zzt_engine_system::Init()
{
    this->ComponentRequest("position_component");
    this->ComponentRequest("text_component");
    this->ComponentRequest("zzt_status_el_component");
    this->ComponentRequest("input_component");
    initZZTpalette();
    initZZTelements();
}

void zzt_engine_system::BoardChange(int16_t board)
{
    this->world->BoardChange(board);
    auto Components = this->ComponentsGet();
    for(auto &[e, tcomponent] : Components["text_component"])
    {
        this->Container->Entity(e)->ComponentDestroy("text_component");
    }

    std::cout << "Finished deleting text_components" << std::endl;
}

bool zzt_engine_system::ElementMove(uint8_t el_id, volatile char direction)
{
    auto worldHeader = this->world->HeaderGet();
    auto board = this->boards[worldHeader.PlayerBoard];
    auto element = board->status_elements[el_id];

    std::cout << "Moving on board: " << (unsigned)worldHeader.PlayerBoard << std::endl;

    std::string entity;

    auto Components = this->ComponentsGet();
    for(auto &[e, zcomponent] : Components["zzt_status_el_component"])
    {
        auto z = std::dynamic_pointer_cast<zzt_status_el_component>(zcomponent);
        if(z->id != el_id) continue;

        /* Looks pretty close to ZZT's speed */
        if(z->last_move > (this->cycle - 1)) return false;
        z->last_move = this->cycle;
        entity = e;
        break;
    }

    auto pcomponent = Components["position_component"][entity];
    auto pos = std::dynamic_pointer_cast<position_component>(pcomponent);

    switch(direction)
    {
        case 'W':
            if(pos->x == 0)
            {
                this->BoardChange(board->board_properties.ExitWest);
                return false;
            }
            pos->x--;
            element->LocationX--;
            break;
        case 'E':
            if(pos->x == 59)
            {
                this->BoardChange(board->board_properties.ExitEast);
                return false;
            }
            pos->x++;
            element->LocationX++;
            break;
        case 'N':
            if(pos->y == 0) return false;
            pos->y--;
            element->LocationY--;
            break;
        case 'S':
            if(pos->y == 24) return false;
            pos->y++;
            element->LocationY++;
            break;
        default:
            break;
    }
    return false;
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

    zztWorldHeader worldHeader = this->world->HeaderGet();
    if(this->boards.size() < worldHeader.NumBoards + 1) this->boards.resize(worldHeader.NumBoards + 1);    

    if(this->boards[worldHeader.PlayerBoard] == nullptr)
    {
        this->BoardLoad(worldHeader.PlayerBoard);
        return;
    }

    auto board = this->boards[worldHeader.PlayerBoard];
    auto player = board->status_elements[0];
    auto Components = this->ComponentsGet();

    for(auto &[entity, icomponent] : Components["input_component"])
    {
        auto breakout = false;
        auto i = std::dynamic_pointer_cast<input_component>(icomponent);

        if(i->event == "key_down")
        {
            switch(i->content["key"].asInt())
            {
                case 'q':
                case 'Q':
                    ECS->Shutdown();
                    break;
                case KEY_LEFT:
                    this->ElementMove(0, 'W');
                    breakout = true;
                    break;
                case KEY_RIGHT:
                    this->ElementMove(0, 'E');
                    break;
                case KEY_UP:
                    this->ElementMove(0, 'N');
                    break;
                case KEY_DOWN:
                    this->ElementMove(0, 'S');
                    break;
                default:
                    std::cout << "Unhandled key: " << std::endl;
                    std::cout << std::hex << i->content["key"].asInt() << std::endl;
                    break;
            }
        }

        this->Container->Entity(entity)->destroy();
        if(breakout) break;
    }

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

void zzt_engine_system::BoardLoadText(int16_t index)
{
}

void zzt_engine_system::BoardLoad(int16_t index)
{
    zztBoard *board = nullptr;
    if(this->boards[index] == nullptr)
    {
        this->boards[index] = this->world->BoardGet(index);
    }
    board = this->boards[index];

    board->HeaderDump();
    board->PropertiesDump();

    uint8_t row = 0, col = 0;
    for(auto &element : board->board_elements)
    {
        if((element & 0xFF) == 0)
        {
            col++;
            if(col > 59)
            {
                col = 0;
                row++;
            }
            continue;
        }

        auto e = this->Container->Entity();

        auto pcomponent = e->Component(ComponentLoader::Create("@metaverse-systems/position_component"));
        auto tcomponent = e->Component(ComponentLoader::Create("@metaverse-systems/text_component"));

        auto p = std::dynamic_pointer_cast<position_component>(pcomponent);
        auto t = std::dynamic_pointer_cast<text_component>(tcomponent);

        p->x = col;
        p->y = row;
        p->z = index;

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
            if((p->x != (element->LocationX - 1)) || (p->y != (element->LocationY - 1)) || (p->z != index)) continue;

            if(Components["zzt_status_el_component"][entity] != nullptr) continue;

            auto e = this->Container->Entity(entity);
            auto zcomponent = e->Component(ComponentLoader::Create("@metaverse-systems/zzt_status_el_component"));
            auto z = std::dynamic_pointer_cast<zzt_status_el_component>(zcomponent);
            z->id = i;
        }
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
