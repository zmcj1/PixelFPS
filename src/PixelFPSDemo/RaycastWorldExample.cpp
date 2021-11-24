//  PixelFPS
//  author : zmcj1
//  date : 2021 / 11 / 24
//  game repo : https://github.com/zmcj1/PixelFPS
//  game engine : https://github.com/OneLoneCoder/olcPixelGameEngine
//  raycast_world extension : https://github.com/OneLoneCoder/olcPixelGameEngine/blob/master/Extensions/olcPGEX_RayCastWorld.h
//  massive massive thx javidx9!

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#define OLC_PGEX_RAYCASTWORLD
#include "olcPGEX_RayCastWorld.h"

using namespace olc;
using namespace std;

class RaycastWorldExampleEngine : public olc::rcw::Engine
{
public:

};

class RaycastWorldExample : public PixelGameEngine
{
public:
    RaycastWorldExample()
    {
        sAppName = "RaycastWorld Example";
    }

    bool OnUserCreate() override
    {
        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override
    {
        return true;
    }

    bool OnUserDestroy() override
    {
        return true;
    }
};

int main()
{
    RaycastWorldExample example;
    if (example.Construct(640, 180, 4, 4))
    {
        example.Start();
    }

    return 0;
}