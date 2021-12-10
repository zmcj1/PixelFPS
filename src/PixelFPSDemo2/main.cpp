#define _CRT_SECURE_NO_WARNINGS

//include library:
#include "olcPixelGameEngine.h"
#include "MinConsoleNative.hpp"
#include <iostream>
#include <ctime>

using namespace olc;
using namespace MinConsoleNative;
using namespace std;

//include current:
#include "Resources.hpp"
#include "olcSprite.hpp"
#include "sObject.hpp"
#include "tool.hpp"
#include "PixelFPSDemo2.hpp"
#include "PixelEditor.hpp"

void Error(const wstring& err_msg)
{
    console.WriteLine(err_msg, { 255, 0, 0 });
    console.ReadLine();
}

int main()
{
    time_t now = time(nullptr);
    tm* local_time = localtime(&now);

    console.SetTitle(L"PixelFPSDemo2 Launcher");
    console.WriteLine(L"Today is: year:" + to_wstring(1900 + local_time->tm_year) + L" month:" + to_wstring(local_time->tm_mon + 1) + L" day:" + to_wstring(local_time->tm_mday), { 198, 125, 173 });
    console.WriteLine();
    console.WriteLine(L"Welcome to PixelFPSDemo2 Launcher!", { 255, 75, 43 });
    console.WriteLine(L"Press 'E' open Editor, Press any other key('F') open Game.", { 107, 198, 237 });

    ConsoleKeyInfo keyInfo = console.ReadKey(false);
    console.WriteLine(L"~:" + String::WcharToWstring(toupper(keyInfo.KeyChar)), { 255, 255, 0 });

    //open editor:
    if (tolower(keyInfo.KeyChar) == L'e')
    {
        wstring resFolderPath = L"../../res/";
        if (!File::Exists(resFolderPath))
        {
            resFolderPath = File::GetDirectoryPath() + L"res\\";
        }

        vector<wstring> fileNames = File::GetFileNamesWithExtension(resFolderPath, L".spr");

        if (fileNames.size() == 0)
        {
            Error(L"Can't find any .spr file in specific folder.");
            return -1;
        }

        //display all .spr files:
        console.WriteLine(L"pls choose file(input number and press 'Enter'):");
        console.WriteLine(L"0.create new .spr file", { 200, 50, 125 });

        for (size_t i = 0; i < fileNames.size(); i++)
        {
            console.WriteLine(to_wstring(i + 1) + L"." + fileNames[i]);
        }

        //choose:
        wstring inputStr = console.ReadLine();
        int chooseIndex = String::ToInt(inputStr);

        if (chooseIndex >= 0 && chooseIndex < fileNames.size() + 1)
        {
            PixelEditor editor;
            editor.workDir = resFolderPath;

            //create new file:
            if (chooseIndex == 0)
            {
                console.WriteLine(L"pls input your new file name(without extension):");

                wstring newFileName = console.ReadLine();

                //extension check
                if (newFileName.find(L".spr") != -1)
                {
                    Error(L"dont include extension!");
                    return -1;
                }

                console.WriteLine(L"pls input sprite width and height(split them by using space):");

                cin >> editor.initialSpriteSizeX >> editor.initialSpriteSizeY;

                editor.spritePath = resFolderPath + newFileName + L".spr";

                editor.spriteName = String::WstringToString(newFileName);
            }
            //open exsist file:
            else
            {
                editor.spritePath = resFolderPath + fileNames[chooseIndex - 1];

                editor.spriteName = String::WstringToString(fileNames[chooseIndex - 1]);

                //ask for resizing sprite:
                console.WriteLine(L"do you want to resize sprite? (Y/N)");

                ConsoleKeyInfo consoleKeyInfo = console.ReadKey(false);
                console.WriteLine(L"~:" + String::WcharToWstring(toupper(consoleKeyInfo.KeyChar)), { 255, 255, 0 });

                if (tolower(consoleKeyInfo.KeyChar) == L'y')
                {
                    console.WriteLine(L"pls input new size of your sprite:(split size by space)");
                    console.WriteLine(L"you can press X to resize in editor window.");
                    cin >> editor.resizeSpriteWidth >> editor.resizeSpriteHeight;
                }
            }

            if (editor.Construct(320, 180, 4, 4))
                editor.Start();
        }
        else
        {
            Error(L"incorrect input!");
            return -1;
        }
    }
    else
    {
        PixelFPSDemo2 game;

        //read config file:
        game.ReadGameSetting();

        game.SetDate(1900 + local_time->tm_year, local_time->tm_mon + 1, local_time->tm_mday, local_time->tm_wday);

        if (game.Construct(320, 180, 4, 4))
            game.Start();
    }

    return 0;
}
