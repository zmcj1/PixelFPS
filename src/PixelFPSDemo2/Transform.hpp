#pragma once

#include "Component.hpp"
#include "olcPixelGameEngine.h"

using namespace olc;

class Transform : public Component
{
    CLASS_DECLARATION(Transform)

public:
    vf2d position;
    vf2d velocity;
    float angle = 0.0f;

public:
    Transform() : Component()
    {
    }

    Transform(vf2d position) : Component()
    {
        this->position = position;
    }

    Transform(float x, float y) : Component()
    {
        this->position = vf2d(x, y);
    }
};