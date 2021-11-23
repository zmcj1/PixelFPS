﻿//  PixelFPS
//  author : zmcj1
//  date : 2021 / 11 / 23
//  game repo : https://github.com/zmcj1/PixelFPS
//  game engine : https://github.com/OneLoneCoder/olcPixelGameEngine
//  massive massive thx javidx9!

#include "String.hpp"
using namespace std;

namespace MinConsoleNative
{
    std::wstring String::Trim(const std::wstring& wstr)
    {
        size_t indexOfFirstNotSpace = wstr.find_first_not_of(L' ');
        size_t indexOfLastNotSpace = wstr.find_last_not_of(L' ');
        if (indexOfFirstNotSpace == string::npos)
        {
            return wstr;
        }

        wstring subStr = wstr.substr(indexOfFirstNotSpace,
            indexOfLastNotSpace - indexOfFirstNotSpace + 1);

        return subStr;
    }

    std::wstring String::TrimStart(const std::wstring& wstr)
    {
        size_t indexOfFirstNotSpace = wstr.find_first_not_of(L' ');
        if (indexOfFirstNotSpace == string::npos)
        {
            return wstr;
        }

        wstring subStr = wstr.substr(indexOfFirstNotSpace,
            wstr.size() - indexOfFirstNotSpace + 1);
        return subStr;
    }

    std::wstring String::TrimEnd(const std::wstring& wstr)
    {
        size_t indexOfLastNotSpace = wstr.find_last_not_of(L' ');
        if (indexOfLastNotSpace == string::npos)
        {
            return wstr;
        }

        wstring subStr = wstr.substr(0, indexOfLastNotSpace + 1);
        return subStr;
    }

    std::wstring String::TrimAll(const std::wstring& wstr)
    {
        wstring res;
        for (size_t i = 0; i < wstr.size(); i++)
        {
            if (wstr[i] != _T(' '))
            {
                res += wstr[i];
            }
        }
        return res;
    }

    std::vector<std::wstring> String::Split(const std::wstring& wstr, const std::wstring& separator)
    {
        vector<wstring> strings;

        size_t pos;
        wstring rest = wstr;

        while ((pos = rest.find(separator)) != wstring::npos)
        {
            strings.push_back(rest.substr(0, pos));
            rest.erase(0, pos + separator.size());
        }
        if (!rest.empty())
        {
            strings.push_back(rest);
        }

        return strings;
    }

    bool String::Compare(const std::string& a, const std::string& b)
    {
        return ::strcmp(a.c_str(), b.c_str()) == 0;
    }

    bool String::Compare(const std::wstring& a, const std::wstring& b)
    {
        return ::wcscmp(a.c_str(), b.c_str()) == 0;
    }

    bool String::CompareIgnoreCase(const std::string& a, const std::string& b)
    {
        return _stricmp(a.c_str(), b.c_str()) == 0;
    }

    bool String::CompareIgnoreCase(const std::wstring& a, const std::wstring& b)
    {
        return _wcsicmp(a.c_str(), b.c_str()) == 0;
    }

    uint String::ToHashCode(const std::string& str)
    {
        uint seed = 131; // 31 131 1313 13131 131313 etc..
        uint hash = 0;
        for (int i = 0; i < str.size(); i++)
        {
            hash = hash * seed + str[i];
        }
        return (hash & 0x7FFFFFFF);
    }

    uint String::ToHashCode(const std::wstring& wstr)
    {
        uint seed = 131; // 31 131 1313 13131 131313 etc..
        uint hash = 0;
        for (int i = 0; i < wstr.size(); i++)
        {
            hash = hash * seed + wstr[i];
        }
        return (hash & 0x7FFFFFFF);
    }

    bool String::ToBool(const std::string& str)
    {
        if (CompareIgnoreCase(str, "true"))
            return true;
        else
            return false;
    }

    bool String::ToBool(const std::wstring& wstr)
    {
        if (CompareIgnoreCase(wstr, _T("true")))
            return true;
        else
            return false;
    }

    int String::ToInt(const std::string& str)
    {
        return ::atoi(str.c_str());
    }

    int String::ToInt(const std::wstring& wstr)
    {
        return ::_wtoi(wstr.c_str());
    }

    std::string String::BoolToString(bool value)
    {
        if (value)
            return "true";
        else
            return "false";
    }

    std::string String::CharToString(char c)
    {
        string str(1, _T('\0'));
        str[0] = c;
        return str;
    }

    std::string String::WstringToString(const std::wstring& wstr, Encoding encoding)
    {
        uint codePage = 0;
        switch (encoding)
        {
        case Encoding::Default:
            codePage = CP_ACP;
            break;
        case Encoding::UTF8:
            codePage = CP_UTF8;
            break;
        }

        int len = WideCharToMultiByte(codePage, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
        char* str = new char[len];
        WideCharToMultiByte(codePage, 0, wstr.c_str(), -1, str, len, NULL, NULL);

        string return_str(str);
        delete[] str;
        return return_str;
    }

    char String::WcharToChar(const wchar& wc, Encoding encoding)
    {
        wchar arr[2];
        arr[0] = wc;
        arr[1] = L'\0';

        string str = WstringToString(wstring(arr), encoding);
        return str[0];
    }

    std::wstring String::BoolToWstring(bool value)
    {
        if (value)
            return _T("true");
        else
            return _T("false");
    }

    std::wstring String::WcharToWstring(wchar wc)
    {
        wstring wstr(1, _T('\0'));
        wstr[0] = wc;
        return wstr;
    }

    std::wstring String::StringToWstring(const std::string& str, Encoding encoding)
    {
        uint codePage = 0;
        switch (encoding)
        {
        case Encoding::Default:
            codePage = CP_ACP;
            break;
        case Encoding::UTF8:
            codePage = CP_UTF8;
            break;
        }

        int len = MultiByteToWideChar(codePage, 0, str.c_str(), -1, NULL, 0);
        wchar* wstr = new wchar[len];
        MultiByteToWideChar(codePage, 0, str.c_str(), -1, wstr, len);

        wstring return_wstr(wstr);
        delete[] wstr;
        return return_wstr;
    }

    wchar String::CharToWchar(const char& c, Encoding encoding)
    {
        char arr[2];
        arr[0] = c;
        arr[1] = '\0';

        wstring wstr = StringToWstring(string(arr), encoding);
        return wstr[0];
    }

    bool String::IsUTF8String(const char* str)
    {
        unsigned int nBytes = 0;
        unsigned char chr = *str;
        bool bAllAscii = true;

        for (unsigned int i = 0; str[i] != '\0'; ++i)
        {
            chr = *(str + i);
            if (nBytes == 0 && (chr & 0x80) != 0)
            {
                bAllAscii = false;
            }

            if (nBytes == 0)
            {
                if (chr >= 0x80)
                {
                    if (chr >= 0xFC && chr <= 0xFD)
                    {
                        nBytes = 6;
                    }
                    else if (chr >= 0xF8)
                    {
                        nBytes = 5;
                    }
                    else if (chr >= 0xF0)
                    {
                        nBytes = 4;
                    }
                    else if (chr >= 0xE0)
                    {
                        nBytes = 3;
                    }
                    else if (chr >= 0xC0)
                    {
                        nBytes = 2;
                    }
                    else
                    {
                        return false;
                    }

                    nBytes--;
                }
            }
            else
            {
                if ((chr & 0xC0) != 0x80)
                {
                    return false;
                }
                nBytes--;
            }
        }

        if (nBytes != 0)
        {
            return false;
        }

        if (bAllAscii)
        {
            return true;
        }

        return true;
    }
}