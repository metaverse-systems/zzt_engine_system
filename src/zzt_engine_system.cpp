#include <zzt_engine_system.hpp>

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
    return config;
}

void zzt_engine_system::Init()
{
    this->ComponentRequest("position_component");
}

void zzt_engine_system::Update()
{
    auto dt = this->DeltaTimeGet();

    this->cycle += dt;

    // 110ms == 9.1Hz
    if(this->cycle < 110) return;
    this->cycle -= 110;

    if(this->world == nullptr)
    {
        if(this->filename.size() == 0) return;

        this->world = new zztWorld(filename);
        this->world->HeaderDump();
        return;
    }

    if(this->board == nullptr)
    {
        this->board = this->world->BoardGet(this->current_board);
        this->board->HeaderDump();
        this->board->PropertiesDump();
        this->board->StatusElementDump();
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
