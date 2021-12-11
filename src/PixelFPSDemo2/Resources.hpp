#pragma once

#include "MinConsoleNative.hpp"
#include <string>

using namespace MinConsoleNative;
using namespace std;

class Resources
{
public:
    template<typename T> static T* Load(const wstring& relativePath, const wstring& resFolderPath, const wstring& fileName)
    {
        wstring path;
        wstring absResFolderPath = relativePath + resFolderPath;

        if (File::Exists(absResFolderPath))
        {
            path = absResFolderPath + fileName;
        }
        else
        {
            path = File::GetDirectoryPath() + resFolderPath + fileName;
        }

        T* instance = new T(path);
        return instance;
    }

    template<typename T> static T* Load(const string& relativePath, const string& resFolderPath, const string& fileName)
    {
        string path;
        string absResFolderPath = relativePath + resFolderPath;

        if (File::Exists(String::StringToWstring(absResFolderPath)))
        {
            path = absResFolderPath + fileName;
        }
        else
        {
            path = String::WstringToString(File::GetDirectoryPath()) + resFolderPath + fileName;
        }

        T* instance = new T(path);
        return instance;
    }

    static string GetPath(const string& relativePath, const string& resFolderPath, const string& fileName)
    {
        string path;
        string absResFolderPath = relativePath + resFolderPath;
        if (File::Exists(String::StringToWstring(absResFolderPath)))
        {
            path = absResFolderPath + fileName;
        }
        else
        {
            path = String::WstringToString(File::GetDirectoryPath()) + resFolderPath + fileName;
        }
        return path;
    }

    //TODO GetPathW
};