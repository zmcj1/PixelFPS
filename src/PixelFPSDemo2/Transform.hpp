#pragma once

#include "Component.hpp"
#include "olcPixelGameEngine.h"

using namespace olc;

class Transform : public Component
{
    CLASS_DECLARATION(Transform)

public:
    vf2d position;

public:
    Transform() : Component()
    {
    }

    Transform(vf2d position) : Component()
    {
        this->position = position;
    }
};