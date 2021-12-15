#pragma once

#include "Component.hpp"
#include "olcPixelGameEngine.h"
#include "EasyBMP.h"
#include "MinConsoleNative.hpp"

using namespace olc;

class BMPRenderer : public Component
{
    CLASS_DECLARATION(BMPRenderer)

public:
    vf2d ObjectSize = vf2d(1, 1);

    BMP* bmp = nullptr;

    int GetWidth()
    {
        return bmp->TellWidth();
    }

    int GetHeight()
    {
        return bmp->TellHeight();
    }

    Color32 SampleColour(float x, float y)
    {
        Color32 color(0, 0, 0, 0);
        int sx = (int)(x * (float)GetWidth());
        int sy = (int)(y * (float)GetHeight());
        if (sx < 0 || sx >= GetWidth() || sy < 0 || sy >= GetHeight())
        {
            return color;
        }
        else
        {
            auto c = bmp->GetPixel(sx, sy);
            color.r = c.Red;
            color.g = c.Green;
            color.b = c.Blue;
            color.a = c.Alpha;
        }
        return color;
    }

    BMPRenderer() : Component()
    {
    }
};