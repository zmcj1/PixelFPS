#pragma once

#include "Component.hpp"
#include "olcPixelGameEngine.h"

using namespace olc;

class LifeController : public Component
{
    CLASS_DECLARATION(LifeController)

public:
    float lifeTime = 1.0f; //just a defualt value

    LifeController() : Component()
    {
    }
};