#pragma once

#include "Component.hpp"
#include "olcPixelGameEngine.h"

using namespace olc;

class Collider : public Component
{
    CLASS_DECLARATION(Collider)

public:
    Collider() : Component()
    {
    }

    //碰撞半径 Collision radius of object
    float radius = 0.5f;

    //是否与其他Collider发生碰撞
    bool collideWithObjects = false;

    //是否可以被其他碰撞体移动
    bool canBeMoved = false;
};