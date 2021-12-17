#pragma once

#include "Component.hpp"
#include "olcPixelGameEngine.h"

using namespace olc;

class PNGRenderer : public Component
{
    CLASS_DECLARATION(PNGRenderer)

public:
    vf2d ObjectSize = vf2d(1, 1);

    olc::Sprite* sprite = nullptr;

    int GetWidth()
    {
        return sprite->width;
    }

    int GetHeight()
    {
        return sprite->height;
    }

    Pixel SampleColour(float x, float y)
    {
        return sprite->Sample(x, y);
    }
};