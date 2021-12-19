#pragma once

#include "Component.hpp"
#include "olcPixelGameEngine.h"

using namespace olc;

class NetworkID : public Component
{
    CLASS_DECLARATION(NetworkID)

public:
    uint32_t ID = 0;

    NetworkID() : Component()
    {
    }
};