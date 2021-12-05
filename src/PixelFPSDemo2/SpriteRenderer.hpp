#pragma once

#include "Component.hpp"
#include "olcPixelGameEngine.h"
#include "OLCSprite.hpp"

using namespace olc;
using namespace MinConsoleNative;

class SpriteRenderer : public Component
{
    CLASS_DECLARATION(SpriteRenderer)

public:
    OLCSprite* sprite;
    
    SpriteRenderer() : Component()
    {
        this->sprite = nullptr;
    }

    SpriteRenderer(OLCSprite* sprite) : Component()
    {
        this->sprite = sprite;
    }
};