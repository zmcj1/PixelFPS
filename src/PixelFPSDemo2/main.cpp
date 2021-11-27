//include library:
#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "MinConsoleNative.hpp"
#include <iostream>

using namespace olc;
using namespace MinConsoleNative;
using namespace std;

//include current:
#include "olcSprite.hpp"
#include "sObject.hpp"
#include "tool.hpp"
#include "PixelFPSDemo2.hpp"
#include "PixelEditor.hpp"

int main()
{
    console.SetTitle(L"PixelFPSDemo2 Console");

    char input_char = '\0';
    console.WriteLine(L"welcome to PixelFPSDemo2!", { 255, 75, 43 });
    console.WriteLine(L"Press 'E' open Editor, Press any other key open Game.", { 107, 198, 237 });
    cin >> input_char;

    if (tolower(input_char) == 'e')
    {
        wstring folderPath = L"../../res/";

        //choose sprite file:
        vector<wstring> fileNames = get_all_files_names_within_folder(folderPath, L"spr");
        //display all .spr files:
        cout << "pls choose file:\n";
        cout << "0.create new file\n";
        for (size_t i = 0; i < fileNames.size(); i++)
        {
            string fileName = String::WstringToString(fileNames[i]);
            cout << i + 1 << "." << fileName << "\n";
        }
        //choose:
        int chooseIndex;
        cin >> chooseIndex;

        if (chooseIndex >= 0 && chooseIndex < fileNames.size() + 1)
        {
            PixelEditor editor;
            wstring finalFilePath;

            //create new file:
            if (chooseIndex == 0)
            {
                cout << "pls input your new file name(without extension):\n";
                string newFileName;
                cin >> newFileName;

                //extension check
                if (newFileName.find(".spr") != -1)
                {
                    cout << "dont include extension!\n";
                    system("pause");
                    return -1;
                }

                cout << "pls input sprite width and height(split them by using space):\n";
                cin >> editor.initialSpriteSizeX >> editor.initialSpriteSizeY;

                //combine:
                finalFilePath = folderPath + String::StringToWstring(newFileName) + L".spr";

                editor.spriteName = newFileName;
            }
            //open exsist file:
            else
            {
                finalFilePath = folderPath + fileNames[chooseIndex - 1];

                editor.spriteName = String::WstringToString(fileNames[chooseIndex - 1]);

                //ask for resizing sprite:
                cout << "do you want to resize sprite? (Y/N)";
                char yes_or_no_char;
                cin >> yes_or_no_char;

                bool yes = false;

                if (tolower(yes_or_no_char) == 'y')
                {
                    yes = true;
                }
                else
                {
                    yes = false;
                }

                if (yes)
                {
                    cout << "pls input new size of your sprite:(split size by space)\n";
                    cout << "you can press X to resize in editor window.\n";
                    cin >> editor.resizeSpriteWidth >> editor.resizeSpriteHeight;
                }
            }

            editor.spritePath = finalFilePath;

            if (editor.Construct(320, 180, 4, 4))
                editor.Start();
        }
        else
        {
            cout << "incorrect input!\n";
            system("pause");
            return -1;
        }
    }
    else
    {
        PixelFPSDemo2 game;

        if (game.Construct(320, 180, 4, 4))
            game.Start();
    }

    return 0;
}
