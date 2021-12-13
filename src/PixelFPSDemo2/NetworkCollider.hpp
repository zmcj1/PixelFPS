#pragma once

#include "Component.hpp"
#include "olcPixelGameEngine.h"

using namespace olc;

class NetworkCollider : public Component
{
    CLASS_DECLARATION(NetworkCollider)

public:
    int networkID;

    NetworkCollider() : Component()
    {
    }

    NetworkCollider(int networkID) : Component()
    {
        this->networkID = networkID;
    }
};