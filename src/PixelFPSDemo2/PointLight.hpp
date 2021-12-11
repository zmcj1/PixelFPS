#pragma once

#include "Component.hpp"
#include "olcPixelGameEngine.h"

using namespace olc;

class PointLight : public Component
{
    CLASS_DECLARATION(PointLight)

public:
    PointLight() : Component()
    {
    }
};