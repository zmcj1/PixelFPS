#pragma once

#include "olcPixelGameEngine.h"
#include "BMP.h"
#include "MinConsoleNative.hpp"
#include <Windows.h>
#include <string>
#include <vector>
#include <cassert>

using namespace std;
using namespace olc;

void debug_output_line(const wstring& info);

//writeline:
void debug_output_vector2(const vi2d& vec2);

//from:https://stackoverflow.com/questions/612097/how-can-i-get-the-list-of-files-in-a-directory-using-c-or-c
//extension without dot(.)
vector<wstring> get_all_files_names_within_folder(const wstring& folder, const wstring& extension);

//https://en.cppreference.com/w/cpp/algorithm/clamp
template<typename T>
constexpr const T& clamp(const T& v, const T& lo, const T& hi)
{
    assert(!(hi < lo));
    return v < lo ? lo : hi < v ? hi : v;
}

inline Color32 getColorFromBMP(const BMP& bmp, int x, int y)
{
    if (x > (uint32_t)bmp.bmp_info_header.width || y > (uint32_t)bmp.bmp_info_header.height)
    {
        throw std::runtime_error("The region does not fit in the image!");
    }

    uint32_t channels = bmp.bmp_info_header.bit_count / 8;

    Color32 color;

    color.b = bmp.data[channels * (y * bmp.bmp_info_header.width + x) + 0];
    color.g = bmp.data[channels * (y * bmp.bmp_info_header.width + x) + 1];
    color.r = bmp.data[channels * (y * bmp.bmp_info_header.width + x) + 2];
    //color.a = 255;
    if (channels == 4)
    {
        color.a = bmp.data[channels * (y * bmp.bmp_info_header.width + x) + 3];
    }

    //color.b = bmp.data[channels * (y * bmp.bmp_info_header.width + x) + 0];
    //color.g = bmp.data[channels * (y * bmp.bmp_info_header.width + x) + 1];
    //color.r = bmp.data[channels * (y * bmp.bmp_info_header.width + x) + 2];
    //color.a = bmp.data[channels * (y * bmp.bmp_info_header.width + x) + 3];
    //if (channels == 4)
    //{
    //    color.a = bmp.data[channels * (y * bmp.bmp_info_header.width + x) + 3];
    //}

    return color;
}
