#pragma once

#include "olcPixelGameEngine.h"
#include <Windows.h>
#include <string>
#include <vector>

using namespace std;
using namespace olc;

void debug_output_line(const wstring& info);

//writeline:
void debug_output_vector2(const vi2d& vec2);

//from:https://stackoverflow.com/questions/612097/how-can-i-get-the-list-of-files-in-a-directory-using-c-or-c
//extension without dot(.)
vector<wstring> get_all_files_names_within_folder(const wstring& folder, const wstring& extension);
