#pragma once

#include "Component.hpp"
#include "olcPixelGameEngine.h"

using namespace olc;

class Collider : public Component
{
    CLASS_DECLARATION(Collider)

public:
    bool enableCollision;

    Collider() : Component()
    {
        enableCollision = true;
    }

    Collider(bool enableCollision) : Component()
    {
        this->enableCollision = enableCollision;
    }
};