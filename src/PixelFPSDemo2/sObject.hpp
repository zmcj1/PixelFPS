#pragma once

#include "olcSprite.hpp"

class sObject
{
public:
    float x;
    float y;
    //velocity:
    float vx = 0;
    float vy = 0;
    bool remove = false;
    OLCSprite* sprite;

    //collision with wall and border of map
    bool enableCollision = true;
    bool enableLifeTime = false;
    float lifeTime = 1.0f; //just a defualt value

    sObject(float x, float y, OLCSprite* sprite)
    {
        this->x = x;
        this->y = y;
        this->sprite = sprite;
    }
};