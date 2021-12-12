#pragma once

#include "Component.hpp"
#include "olcPixelGameEngine.h"

using namespace olc;

class PointLight : public Component
{
    CLASS_DECLARATION(PointLight)

public:
    float range = 1.0f;
    float attenuation = 0.0f;

    PointLight() : Component()
    {
    }

    PointLight(float range) : Component()
    {
        this->range = range;
    }
};