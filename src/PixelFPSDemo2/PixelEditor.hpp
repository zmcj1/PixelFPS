#pragma once

//editor for olcSprite and Color32!
class PixelEditor : public PixelGameEngine
{
public:
    string spriteName = "unknown"; //include extension .spr
    wstring spritePath;

    int initialSpriteSizeX = 32;
    int initialSpriteSizeY = 32;

    int resizeSpriteWidth = 32;
    int resizeSpriteHeight = 32;

private:
    //palette:
    std::map<ConsoleColor, Color24> palette;
    //sprite pointer:
    olcSprite* spritePtr;

    int defaultSpritePosX = 20;
    int defaultSpritePosY = 20;
    int defaultZoomPixelScaler = 1;

    int spritePosX = defaultSpritePosX;
    int spritePosY = defaultSpritePosY;

    //palette setting props:
    //-1 means you didn't choose any color yet.
    int chosenPaletteColorIndex = -1;
    bool chosenErase = false;

    int paletteColorSizeX = 10;
    int paletteColorSizeY = 10;

    int prevMousePosX = 0;
    int prevMousePosY = 0;

    //save setting:
    bool isDirty = false;

    //play music while painting :D
    Audio* theme_of_laura = nullptr;

    void DisplaySprite(const wstring& path, int x, int y, int zoomPixelScaler)
    {
        //load olcSprite:
        olcSprite sprite(path);
        DisplaySprite(&sprite, x, y, zoomPixelScaler);
    }

    void DisplaySprite(olcSprite* sprite, int x, int y, int zoomPixelScaler)
    {
        //draw with scaler:
        for (size_t i = 0; i < sprite->nHeight * zoomPixelScaler; i++)
        {
            for (size_t j = 0; j < sprite->nWidth * zoomPixelScaler; j++)
            {
                short c = sprite->GetGlyph(j / zoomPixelScaler, i / zoomPixelScaler);

                //ignore alpha
                if (c == L' ')
                {
                    continue;
                }

                short att = sprite->GetColour(j / zoomPixelScaler, i / zoomPixelScaler);
                ConsoleColor foreColor = (ConsoleColor)(att & 0x000F);
                ConsoleColor backColor = (ConsoleColor)((att & 0x00F0) / 16);

                Color24 pixelColor = palette[foreColor];
                UNUSED(backColor);

                Draw(x + j, y + i, Pixel(pixelColor.r, pixelColor.g, pixelColor.b));
            }
        }
    }

    ushort ToUshort(ConsoleColor foreColor, ConsoleColor backColor)
    {
        return (ushort)((ushort)foreColor | ((ushort)backColor << 4));
    }

public:
    const wstring PxsFolder = L"../../res/pxs/";

    static wstring GetIndentValue(MinConsoleNative::byte value)
    {
        wstring result;

        if (value / 100 == 0) //必定是2位数
        {
            int indent = 1;
            if (value / 10 == 0) //必定是1位数
            {
                indent++;
            }
            for (int k = 0; k < indent; k++)
            {
                result += L" ";
            }
        }

        result += to_wstring(value);
        return result;
    }

    static vector<wstring> ConvertSprToPxs(olcSprite* sprite, std::map<ConsoleColor, Color24>& palette)
    {
        vector<wstring> lines;

        //first line:
        lines.push_back(to_wstring(sprite->nWidth) + L"," + to_wstring(sprite->nHeight));

        //color32 lines:
        for (int y = 0; y < sprite->nHeight; y++)
        {
            wstring line;
            for (int x = 0; x < sprite->nWidth; x++)
            {
                short att = sprite->GetColour(x, y);
                short c = sprite->GetGlyph(x, y);

                ConsoleColor foreColor = (ConsoleColor)(att & 0x000F);
                ConsoleColor backColor = (ConsoleColor)((att & 0x00F0) / 16);

                Color24 pixelColor = palette[foreColor];
                UNUSED(backColor);

                Color32 saveColor(pixelColor.r, pixelColor.g, pixelColor.b, 255);

                //set alpha to 0
                if (c == ' ')
                {
                    saveColor.a = 0;
                }

                //normal:
                //line += L"(" + to_wstring(saveColor.r) + L"," + to_wstring(saveColor.g) + L"," + to_wstring(saveColor.b) + L"," + to_wstring(saveColor.a) + L"),";
                //add indent:
                line += L"(" + GetIndentValue(saveColor.r) + L"," + GetIndentValue(saveColor.g) + L"," + GetIndentValue(saveColor.b) + L"," + GetIndentValue(saveColor.a) + L"),";
            }
            lines.push_back(line);
        }

        return lines;
    }

public:
    PixelEditor()
    {
        sAppName = "Pixel Editor";
    }

    bool OnUserCreate() override
    {
        this->palette[ConsoleColor::BLACK] = { 0, 0, 0 };
        this->palette[ConsoleColor::DARKBLUE] = { 0, 0, 128 };
        this->palette[ConsoleColor::DARKGREEN] = { 0, 128, 0 };
        this->palette[ConsoleColor::DARKCYAN] = { 0, 128, 128 };
        this->palette[ConsoleColor::DARKRED] = { 128, 0, 0 };
        this->palette[ConsoleColor::DARKMAGENTA] = { 128, 0, 128 };
        this->palette[ConsoleColor::DARKYELLOW] = { 128, 128, 0 };
        this->palette[ConsoleColor::GRAY] = { 192, 192, 192 };
        this->palette[ConsoleColor::DARKGRAY] = { 128, 128, 128 };
        this->palette[ConsoleColor::BLUE] = { 0, 0, 255 };
        this->palette[ConsoleColor::GREEN] = { 0, 255, 0 };
        this->palette[ConsoleColor::CYAN] = { 0, 255, 255 };
        this->palette[ConsoleColor::RED] = { 255, 0, 0 };
        this->palette[ConsoleColor::MAGENTA] = { 255, 0, 255 };
        this->palette[ConsoleColor::YELLOW] = { 255, 255, 0 };
        this->palette[ConsoleColor::WHITE] = { 255, 255, 255 };

        //empty check
        if (spritePath == L"")
        {
            return false;
        }

        if (!File::Exists(spritePath))
        {
            spritePtr = new olcSprite(initialSpriteSizeX, initialSpriteSizeY);
            //save
            spritePtr->Save(spritePath);
        }
        else
        {
            spritePtr = new olcSprite(spritePath);
        }

        theme_of_laura = new Audio(L"../../res/audios/Theme of Laura.mp3");
        theme_of_laura->Play(true, false);

        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override
    {
        int mw = GetMouseWheel();

        if (mw > 0)
        {
            this->defaultZoomPixelScaler++;
        }
        else if (mw < 0)
        {
            this->defaultZoomPixelScaler--;
            //at least 1
            if (this->defaultZoomPixelScaler < 1)
            {
                this->defaultZoomPixelScaler = 1;
            }
        }

        //reset:
        if (GetKey(Key::R).bPressed)
        {
            this->spritePosX = defaultSpritePosX;
            this->spritePosY = defaultSpritePosY;
            this->defaultZoomPixelScaler = 1;
        }

        //choose erase:
        if (GetKey(Key::E).bPressed)
        {
            chosenErase = true;
        }

        //choose color:
        if (GetKey(Key::B).bPressed)
        {
            chosenErase = false;
        }

        //save:
        if (GetKey(Key::S).bPressed)
        {
            spritePtr->Save(spritePath);

            //save as .pxs file:
            vector<wstring> pxs_lines = ConvertSprToPxs(this->spritePtr, palette);

            string spriteNameWithoutEx = this->spriteName.substr(0, this->spriteName.find('.'));
            wstring saveFileName = String::StringToWstring(spriteNameWithoutEx) + L".pxs";

            wstring __path = PxsFolder + saveFileName;

            if (!File::Exists(__path))
            {
                File::Creat(__path);
            }
            File::WriteAllLines(__path, pxs_lines);

            this->isDirty = false;
        }

        //resize:
        if (GetKey(Key::X).bPressed)
        {
            this->spritePtr->Resize(resizeSpriteWidth, resizeSpriteHeight);
            this->isDirty = true;
        }

        int mouseX = GetMouseX();
        int mouseY = GetMouseY();

        //choose color:
        if (mouseX >= 0 && mouseX < paletteColorSizeX &&
            mouseY >= 0 && mouseY < paletteColorSizeY * palette.size())
        {
            if (GetMouse(Mouse::LEFT).bHeld)
            {
                this->chosenPaletteColorIndex = mouseY / paletteColorSizeY;
                this->chosenErase = false;
            }
        }

        int selectIndexX = -1;
        int selectIndexY = -1;

        //mouse pos in sprite:
        if (mouseX >= spritePosX && mouseY >= spritePosY &&
            mouseX < spritePosX + spritePtr->nWidth * defaultZoomPixelScaler &&
            mouseY < spritePosY + spritePtr->nHeight * defaultZoomPixelScaler)
        {
            int mouseInSpritePosX = mouseX - spritePosX;
            int mouseInSpritePosY = mouseY - spritePosY;
            selectIndexX = mouseInSpritePosX / defaultZoomPixelScaler;
            selectIndexY = mouseInSpritePosY / defaultZoomPixelScaler;

            //NOTE!!! 注意该API并不会获取真实鼠标物理状态, 如果按住鼠标的同时把鼠标移出窗口则会导致鼠标状态无法及时更新
            if (GetMouse(Mouse::RIGHT).bHeld)
            {
                //drag sprite:
                if (prevMousePosX != 0 && prevMousePosY != 0)
                {
                    int diffX = mouseX - prevMousePosX;
                    int diffY = mouseY - prevMousePosY;

                    spritePosX += diffX;
                    spritePosY += diffY;
                }

                prevMousePosX = mouseX;
                prevMousePosY = mouseY;
            }
            else
            {
                prevMousePosX = 0;
                prevMousePosY = 0;
            }

            //select pixel(drawing):
            if (GetMouse(Mouse::LEFT).bHeld)
            {
                //draw alpha(erase):
                if (this->chosenErase)
                {
                    //set it to 0:
                    this->spritePtr->SetColour(selectIndexX, selectIndexY, 0);
                    //disable draw this pixel:
                    this->spritePtr->SetGlyph(selectIndexX, selectIndexY, ' ');
                    //set dirty sign:
                    this->isDirty = true;
                }
                else
                {
                    if (this->chosenPaletteColorIndex != -1)
                    {
                        ushort att = ToUshort((ConsoleColor)this->chosenPaletteColorIndex, ConsoleColor::BLACK);
                        this->spritePtr->SetColour(selectIndexX, selectIndexY, att);
                        //fill
                        this->spritePtr->SetGlyph(selectIndexX, selectIndexY, 'a');
                        //set dirty sign:
                        this->isDirty = true;
                    }
                }
            }

            //drag pixel of sprite:
            if (GetMouse(Mouse::MIDDLE).bHeld)
            {
                //todo:
                // 
                // 
                //short* changedGlyphs = new short[this->spritePtr->nWidth * this->spritePtr->nHeight];
                //short* changedColours = new short[this->spritePtr->nWidth * this->spritePtr->nHeight];
                //olcSprite* buffer = new olcSprite(this->spritePtr->nWidth, this->spritePtr->nHeight);
                //for (int y = 0; y < this->spritePtr->nHeight; y++)
                //{
                //    for (int x = 0; x < this->spritePtr->nWidth; x++)
                //    {
                //        short glyph = this->spritePtr->GetGlyph(x, y);
                //        short color = this->spritePtr->GetColour(x, y);
                //        buffer->SetGlyph(x, y, glyph);
                //        buffer->SetColour(x, y, color);
                //    }
                //}
            }
        }
        else
        {
            prevMousePosX = 0;
            prevMousePosY = 0;
        }

        //clear screen:
        this->Clear(Pixel(44, 44, 44));

        string text = "Welcome to Pixel Editor";
        string text2 = "Chosen Color:";
        string text3 = "Size:";
        string text4 = "Sprite Name:";

        vi2d textSize = GetTextSize(text);
        vi2d text2Size = GetTextSize(text2);
        //vi2d text3Size = GetTextSize(text3);
        //vi2d text4Size = GetTextSize(text4);

        //draw texts:
        DrawString({ 50, 0 }, text);
        DrawString({ 50, textSize.y }, text2);
        //draw dirty sign:
        if (isDirty)
        {
            DrawString({ 50 + textSize.x, 0 }, "*");
        }
        //draw sprite size:
        DrawString({ 50, textSize.y * 2 }, text3 + to_string(spritePtr->nWidth) + "X" + to_string(spritePtr->nHeight));
        //draw sprite name
        DrawString({ 50, textSize.y * 3 }, text4 + this->spriteName);
        //draw current cursor pos in sprite:
        DrawString({ 50, textSize.y * 4 }, "(" + to_string(selectIndexX) + ", " + to_string(selectIndexY) + ")");

        //choosen is erase:
        if (this->chosenErase)
        {
            DrawString({ 50 + text2Size.x, text2Size.y }, "erase", Pixel(125, 0, 66));
        }
        //draw choosen color:
        else
        {
            if (this->chosenPaletteColorIndex != -1)
            {
                for (int y = 0; y < paletteColorSizeY; y++)
                {
                    for (int x = 0; x < paletteColorSizeX; x++)
                    {
                        Color24 color = palette[(ConsoleColor)this->chosenPaletteColorIndex];
                        Draw({ x + 50 + text2Size.x, y + text2Size.y }, Pixel(color.r, color.g, color.b));
                    }
                }
            }
        }

        //draw sprite bg(ex info for painting):
        for (int i = 0; i < spritePtr->nHeight * defaultZoomPixelScaler; i++)
        {
            for (int j = 0; j < spritePtr->nWidth * defaultZoomPixelScaler; j++)
            {
                Draw(j + spritePosX, i + spritePosY, Pixel(88, 88, 88));
            }
        }

        //draw sprite:
        DisplaySprite(spritePtr, spritePosX, spritePosY, defaultZoomPixelScaler);

        //draw painting preview:
        if (selectIndexX != -1 && selectIndexY != -1)
        {
            for (int i = 0; i < defaultZoomPixelScaler; i++)
            {
                for (int j = 0; j < defaultZoomPixelScaler; j++)
                {
                    //erase:
                    if (this->chosenErase)
                    {
                        Color24 color = Color24(88, 88, 88);
                        Draw({
                            j + selectIndexX * defaultZoomPixelScaler + spritePosX,
                            i + selectIndexY * defaultZoomPixelScaler + spritePosY },
                            Pixel(color.r, color.g, color.b));
                    }
                    //color:
                    else
                    {
                        if (this->chosenPaletteColorIndex != -1)
                        {
                            Color24 color = palette[(ConsoleColor)this->chosenPaletteColorIndex];
                            Draw({
                                j + selectIndexX * defaultZoomPixelScaler + spritePosX,
                                i + selectIndexY * defaultZoomPixelScaler + spritePosY },
                                Pixel(color.r, color.g, color.b));
                        }
                    }
                }
            }
        }

        //draw palette color:
        for (int i = 0; i < palette.size(); i++)
        {
            for (int y = 0; y < paletteColorSizeY; y++)
            {
                for (int x = 0; x < paletteColorSizeX; x++)
                {
                    Color24 color = palette[(ConsoleColor)i];
                    Draw({ x, y + i * paletteColorSizeY }, Pixel(color.r, color.g, color.b));
                }
            }
        }

        return true;
    }

    bool OnUserDestroy() override
    {
        delete this->spritePtr;
        delete this->theme_of_laura;

        return true;
    }
};
