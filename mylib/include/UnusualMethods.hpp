﻿#pragma once

#include "MinDefines.hpp"

//Here are some functions with very low usage.

namespace MinConsoleNative
{
    class UnusualMethods
    {
    private:
        UnusualMethods()
        {
        }

    public:
        static constexpr const wchar* Kernel32DLL = L"kernel32.dll";

        static bool GetFuncFromDll(const wchar* dllName, const char* funcName, FARPROC* funcPtr);

        static bool SetConsoleIcon(HICON hIcon);
    };
}