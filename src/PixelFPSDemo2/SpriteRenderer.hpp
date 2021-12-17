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
    vf2d ObjectSize = vf2d(1, 1);
    vf2d ObjectPos = vf2d(0, 0);

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