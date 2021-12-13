#include "tool.hpp"

namespace fuck_std
{
    void debug_output_line(const wstring& info)
    {
        OutputDebugString((info + L"\n").c_str());
    }

    //writeline:
    void debug_output_vector2(const vi2d& vec2)
    {
        debug_output_line(to_wstring(vec2.x) + L" " + to_wstring(vec2.y));
    }

    //writeline:
    void debug_output_vector2f(const vf2d& vec2f)
    {
        debug_output_line(to_wstring(vec2f.x) + L" " + to_wstring(vec2f.y));
    }

    //from:https://stackoverflow.com/questions/612097/how-can-i-get-the-list-of-files-in-a-directory-using-c-or-c
    //extension without dot(.)
    vector<wstring> get_all_files_names_within_folder(const wstring& folder, const wstring& extension)
    {
        vector<wstring> names;
        wstring search_path = folder + L"/*." + extension;
        WIN32_FIND_DATA fd;
        HANDLE hFind = ::FindFirstFile(search_path.c_str(), &fd);
        if (hFind != INVALID_HANDLE_VALUE)
        {
            do
            {
                // read all (real) files in current folder
                // , delete '!' read other 2 default folder . and ..
                if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
                {
                    names.push_back(fd.cFileName);
                }
            }
            while (::FindNextFile(hFind, &fd));
            ::FindClose(hFind);
        }
        return names;
    }
};
