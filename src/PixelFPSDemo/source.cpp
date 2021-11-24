//  PixelFPS
//  author : zmcj1
//  date : 2021 / 11 / 24
//  game repo : https://github.com/zmcj1/PixelFPS
//  game engine : https://github.com/OneLoneCoder/olcPixelGameEngine
//  massive massive thx javidx9!

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "Audio.hpp"
#include "AudioPool.hpp"
#include "File.hpp"
#include <cmath>
#include <list>
#include <map>

using namespace olc;
using namespace std;
using namespace MinConsoleNative;

enum class ConsoleColor
{
    UNKNOWN = -1,
    BLACK = 0,
    DARKBLUE = 1,
    DARKGREEN = 2,
    DARKCYAN = 3,
    DARKRED = 4,
    DARKMAGENTA = 5,
    DARKYELLOW = 6,
    GRAY = 7,
    DARKGRAY = 8,
    BLUE = 9,
    GREEN = 10,
    CYAN = 11,
    RED = 12,
    MAGENTA = 13,
    YELLOW = 14,
    WHITE = 15,
};

typedef unsigned char byte;

struct Color24
{
public:
    ::byte r;
    ::byte g;
    ::byte b;

    Color24()
    {
        this->r = 0;
        this->g = 0;
        this->b = 0;
    }

    Color24(::byte r, ::byte g, ::byte b)
    {
        this->r = r;
        this->g = g;
        this->b = b;
    }
};

class olcSprite
{
public:
    int nWidth = 0;
    int nHeight = 0;

    olcSprite()
    {

    }

    olcSprite(int w, int h)
    {
        Create(w, h);
    }

    olcSprite(const std::wstring& sFile)
    {
        if (!Load(sFile))
            Create(8, 8);
    }

private:
    short* m_Glyphs = nullptr;
    short* m_Colours = nullptr;

    void Create(int w, int h)
    {
        nWidth = w;
        nHeight = h;
        m_Glyphs = new short[w * h];
        m_Colours = new short[w * h];
        for (int i = 0; i < w * h; i++)
        {
            m_Glyphs[i] = L' ';
            m_Colours[i] = 0;
        }
    }

public:
    void SetGlyph(int x, int y, short c)
    {
        if (x < 0 || x >= nWidth || y < 0 || y >= nHeight)
            return;
        else
            m_Glyphs[y * nWidth + x] = c;
    }

    void SetColour(int x, int y, short c)
    {
        if (x < 0 || x >= nWidth || y < 0 || y >= nHeight)
            return;
        else
            m_Colours[y * nWidth + x] = c;
    }

    short GetGlyph(int x, int y)
    {
        if (x < 0 || x >= nWidth || y < 0 || y >= nHeight)
            return L' ';
        else
            return m_Glyphs[y * nWidth + x];
    }

    short GetColour(int x, int y)
    {
        if (x < 0 || x >= nWidth || y < 0 || y >= nHeight)
            return 0;
        else
            return m_Colours[y * nWidth + x];
    }

    short SampleGlyph(float x, float y)
    {
        int sx = (int)(x * (float)nWidth);
        int sy = (int)(y * (float)nHeight - 1.0f);
        if (sx < 0 || sx >= nWidth || sy < 0 || sy >= nHeight)
            return L' ';
        else
            return m_Glyphs[sy * nWidth + sx];
    }

    short SampleColour(float x, float y)
    {
        int sx = (int)(x * (float)nWidth);
        int sy = (int)(y * (float)nHeight - 1.0f);
        if (sx < 0 || sx >= nWidth || sy < 0 || sy >= nHeight)
            return 0;
        else
            return m_Colours[sy * nWidth + sx];
    }

    bool Save(std::wstring sFile)
    {
        FILE* f = nullptr;
        _wfopen_s(&f, sFile.c_str(), L"wb");
        if (f == nullptr)
            return false;

        fwrite(&nWidth, sizeof(int), 1, f);
        fwrite(&nHeight, sizeof(int), 1, f);
        fwrite(m_Colours, sizeof(short), nWidth * nHeight, f);
        fwrite(m_Glyphs, sizeof(short), nWidth * nHeight, f);

        fclose(f);

        return true;
    }

    bool Load(const std::wstring& sFile)
    {
        delete[] m_Glyphs;
        delete[] m_Colours;
        nWidth = 0;
        nHeight = 0;

        FILE* f = nullptr;
        _wfopen_s(&f, sFile.c_str(), L"rb");
        if (f == nullptr)
            return false;

        std::fread(&nWidth, sizeof(int), 1, f);
        std::fread(&nHeight, sizeof(int), 1, f);

        Create(nWidth, nHeight);

        std::fread(m_Colours, sizeof(short), nWidth * nHeight, f);
        std::fread(m_Glyphs, sizeof(short), nWidth * nHeight, f);

        std::fclose(f);
        return true;
    }
};

class sObject
{
public:
    float x;
    float y;
    //velocity:
    float vx = 0;
    float vy = 0;
    bool remove = false;
    olcSprite* sprite;

    //collision with wall and border of map
    bool enableCollision = true;
    bool enableLifeTime = false;
    float lifeTime = 1.0f; //just a defualt value

    sObject(float x, float y, olcSprite* sprite)
    {
        this->x = x;
        this->y = y;
        this->sprite = sprite;
    }
};

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

class PixelFPSDemo : public PixelGameEngine
{
private:
    //map:
    wstring map;
    int mapWidth = 32;
    int mapHeight = 32;

    //player:
    float playerX = 8.5f;           // Player Start Position
    float playerY = 14.7f;
    float playerAngle = 0.0f;       // Player Start Rotation(in rad)
    float moveSpeed = 5.0f;         // Walking Speed
    float rotateSpeed = 3.14159f;   // Rotating Speed (1 sec 180 degrees)

    //camera:
    const float FOV = 3.14159f / 4; // Field of view
    const float depth = 32.0f;      // Maximum rendering distance

    //sprites:
    olcSprite* spriteWall;
    olcSprite* spriteLamp;
    olcSprite* spriteFireBall;
    olcSprite* spriteExplosion;

    //objects:
    list<sObject> listObjects;

    //palette:
    std::map<ConsoleColor, Color24> palette;

    //depth buffer:
    float* fDepthBuffer = nullptr;

    //audios:
    Audio* bgm = nullptr;
    Audio* bgm2 = nullptr;
    bool startedPlayBGM = false;

    //sounds:
    Audio* explosionSound = nullptr;
    Audio* fireBallSound = nullptr;
    AudioPool* explosionPool = nullptr;
    AudioPool* fireBallPool = nullptr;

public:
    PixelFPSDemo()
    {
        this->sAppName = "PixelFPS Demo";
    }

    // Called once on application startup, use to load your resources
    bool OnUserCreate() override
    {
        //create map:
        map += L"################################";
        map += L"#...............#..............#";
        map += L"#.......#########.......########";
        map += L"#..............##..............#";
        map += L"#......##......##......##......#";
        map += L"#......##..............##......#";
        map += L"#..............##..............#";
        map += L"###............####............#";
        map += L"##.............###.............#";
        map += L"#............####............###";
        map += L"#..............................#";
        map += L"#..............##..............#";
        map += L"#..............##..............#";
        map += L"#...........#####...........####";
        map += L"#..............................#";
        map += L"###..####....########....#######";
        map += L"####.####.......######.........#";
        map += L"#...............#..............#";
        map += L"#.......#########.......##..####";
        map += L"#..............##..............#";
        map += L"#......##......##.......#......#";
        map += L"#......##......##......##......#";
        map += L"#..............##..............#";
        map += L"###............####............#";
        map += L"##.............###.............#";
        map += L"#............####............###";
        map += L"#..............................#";
        map += L"#..............................#";
        map += L"#..............##..............#";
        map += L"#...........##..............####";
        map += L"#..............##..............#";
        map += L"################################";

        //load res:
        this->spriteWall = new olcSprite(L"../../res/fps_wall1.spr");
        this->spriteLamp = new olcSprite(L"../../res/fps_lamp1.spr");
        this->spriteFireBall = new olcSprite(L"../../res/fps_fireball1.spr");
        this->spriteExplosion = new olcSprite(L"../../res/fps_explosion.spr");

        listObjects =
        {
            sObject(8.5f, 8.5f, this->spriteLamp),
            sObject(7.5f, 7.5f, this->spriteLamp),
            sObject(10.5f, 3.5f, this->spriteLamp),
        };

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

        this->fDepthBuffer = new float[ScreenWidth()];

        this->bgm = new Audio(L"../../res/audios/[CSO] Zombie Scenario - Normal Fight.mp3");
        this->bgm2 = new Audio(L"../../res/audios/[CSO] Zombie Scenario - Round Start.mp3");
        this->explosionSound = new Audio(L"../../res/audios/548_Effect.Explosion.wav.mp3");
        this->fireBallSound = new Audio(L"../../res/audios/560_Weapon.Rocket.Fire.wav.mp3");

        this->explosionPool = new AudioPool(L"../../res/audios/548_Effect.Explosion.wav.mp3");
        this->fireBallPool = new AudioPool(L"../../res/audios/560_Weapon.Rocket.Fire.wav.mp3");

        this->bgm2->SetVolume(MCI_MAX_VOLUME / 3);
        this->bgm2->Play(false, false);

        return true;
    }

    // Called every frame, and provides you with a time per frame value
    bool OnUserUpdate(float fElapsedTime) override
    {
        float deltaTime = fElapsedTime;

        //check audio state:
        if (this->bgm2->IsOver() && !startedPlayBGM)
        {
            startedPlayBGM = true;

            this->bgm->SetVolume(MCI_MAX_VOLUME / 3);
            this->bgm->Play(true, false);
        }

        //clean audiopool:
        explosionPool->Clean();
        fireBallPool->Clean();

        if (GetKey(Key::ESCAPE).bPressed)
        {
            return false;
        }

        //rotation
        if (GetKey(Key::A).bHeld)
        {
            playerAngle -= rotateSpeed * deltaTime;
        }
        if (GetKey(Key::D).bHeld)
        {
            playerAngle += rotateSpeed * deltaTime;
        }

        //movement forward
        if (GetKey(Key::W).bHeld)
        {
            float x = ::cosf(playerAngle) * moveSpeed * deltaTime;
            float y = ::sinf(playerAngle) * moveSpeed * deltaTime;
            playerX += x;
            playerY += y;
            if (this->map.c_str()[(int)playerY * mapWidth + (int)playerX] == L'#')
            {
                playerX -= x;
                playerY -= y;
            }
        }

        //movement backward
        if (GetKey(Key::S).bHeld)
        {
            float x = ::cosf(playerAngle) * moveSpeed * deltaTime;
            float y = ::sinf(playerAngle) * moveSpeed * deltaTime;
            playerX -= x;
            playerY -= y;
            if (this->map.c_str()[(int)playerY * mapWidth + (int)playerX] == L'#')
            {
                playerX += x;
                playerY += y;
            }
        }

        //strafe left
        if (GetKey(Key::Q).bHeld)
        {
            float x = ::cosf(playerAngle) * moveSpeed * deltaTime;
            float y = ::sinf(playerAngle) * moveSpeed * deltaTime;
            playerX += y;
            playerY -= x;
            if (this->map.c_str()[(int)playerY * mapWidth + (int)playerX] == L'#')
            {
                playerX -= y;
                playerY += x;
            }
        }

        //strafe right
        if (GetKey(Key::E).bHeld)
        {
            float x = ::cosf(playerAngle) * moveSpeed * deltaTime;
            float y = ::sinf(playerAngle) * moveSpeed * deltaTime;
            playerX -= y;
            playerY += x;
            if (this->map.c_str()[(int)playerY * mapWidth + (int)playerX] == L'#')
            {
                playerX += y;
                playerY -= x;
            }
        }

        //fire:
        if (GetKey(Key::SPACE).bPressed)
        {
            sObject fireBall(playerX, playerY, this->spriteFireBall);

            //set velocity:
            //fireBall.vx = cosf(playerAngle) * 8.0f;
            //fireBall.vy = sinf(playerAngle) * 8.0f;

            //make noise:
            float fNoise = (((float)rand() / (float)RAND_MAX) - 0.5f) * 0.1f;
            fireBall.vx = cosf(playerAngle + fNoise) * 8.0f;
            fireBall.vy = sinf(playerAngle + fNoise) * 8.0f;

            //add to list
            listObjects.push_back(fireBall);

            //play fire sound:
            fireBallPool->PlayOneShot(0.5f);
        }

        //raycast
        for (int x = 0; x < ScreenWidth(); x++)
        {
            float rayAngle = (playerAngle - FOV / 2.0f) + ((float)x / ScreenWidth()) * FOV;

            float stepSize = 0.01f;
            float distanceToWall = 0.0f;

            bool hitWall = false;

            float eyeX = ::cosf(rayAngle);
            float eyeY = ::sinf(rayAngle);

            float sampleX = 0.0f;

            while (!hitWall && distanceToWall < depth)
            {
                distanceToWall += stepSize;

                int testX = (int)(playerX + eyeX * distanceToWall);
                int testY = (int)(playerY + eyeY * distanceToWall);

                if (testX < 0 || testX >= mapWidth || testY < 0 || testY >= mapHeight)
                {
                    hitWall = true;
                    distanceToWall = depth;
                }
                //与墙壁发生了碰撞
                else if (map[testY * mapWidth + testX] == L'#')
                {
                    hitWall = true;

                    //墙壁中心点(假设每一堵墙壁都是1X1大小)
                    float blockMidX = (float)testX + 0.5f;
                    float blockMidY = (float)testY + 0.5f;
                    //射线命中点
                    float testPointX = playerX + eyeX * distanceToWall;
                    float testPointY = playerY + eyeY * distanceToWall;

                    //testAngle为从block中点出发, 射向射线命中点的向量与X轴的夹角
                    float testAngle = atan2f(testPointY - blockMidY, testPointX - blockMidX);

                    //命中右方
                    if (testAngle >= -3.14159f * 0.25f && testAngle < 3.14159f * 0.25f)
                        sampleX = testPointY - (float)testY;
                    //命中上方
                    if (testAngle >= 3.14159f * 0.25f && testAngle < 3.14159f * 0.75f)
                        sampleX = testPointX - (float)testX;
                    //命中下方
                    if (testAngle < -3.14159f * 0.25f && testAngle >= -3.14159f * 0.75f)
                        sampleX = testPointX - (float)testX;
                    //命中左方
                    if (testAngle >= 3.14159f * 0.75f || testAngle < -3.14159f * 0.75f)
                        sampleX = testPointY - (float)testY;
                }
            }

            int ceiling = (int)(ScreenHeight() / 2.0f - ScreenHeight() / distanceToWall );
            int floor = ScreenHeight() - ceiling;

            //update depth buffer:
            fDepthBuffer[x] = distanceToWall;

            for (int y = 0; y < ScreenHeight(); y++)
            {
                //draw ceiling
                if (y <= ceiling)
                {
                    Draw(x, y, Pixel(0, 0, 255));
                }
                //draw wall
                else if (y > ceiling && y <= floor)
                {
                    if (distanceToWall < depth)
                    {
                        float sampleY = ((float)y - (float)ceiling) / ((float)floor - (float)ceiling);

                        short att = spriteWall->SampleColour(sampleX, sampleY);

                        ConsoleColor foreColor = (ConsoleColor)(att & 0x000F);
                        ConsoleColor backColor = (ConsoleColor)((att & 0x00F0) / 16);

                        Color24 pixelColor = palette[foreColor];
                        UNUSED(backColor);

                        Draw(x, y, Pixel(pixelColor.r, pixelColor.g, pixelColor.b));
                    }
                    else
                    {
                        Draw(x, y, Pixel(0, 0, 0));
                    }
                }
                //draw floor
                else
                {
                    Draw(x, y, Pixel(0, 128, 0));
                }
            }
        }

        //draw objects:
        for (auto& object : listObjects)
        {
            //update physics:
            object.x += object.vx * deltaTime;
            object.y += object.vy * deltaTime;

            //lifetime detect:
            if (object.enableLifeTime)
            {
                object.lifeTime -= deltaTime;
                if (object.lifeTime <= 0)
                {
                    object.remove = true;
                }
            }

            //collision detect:
            if (object.enableCollision)
            {
                // Check if object is inside wall - set flag for removal
                if (object.x >= 0 && object.x < mapWidth && object.y >= 0 && object.y < mapHeight)
                {
                    //collsion with walls:
                    if (map[(int)object.y * mapWidth + (int)object.x] == L'#')
                    {
                        object.remove = true;

                        //play explosion sound:
                        explosionPool->PlayOneShot();

                        //instantiate explosion:
                        sObject explosion(
                            object.x - object.vx * deltaTime,
                            object.y - object.vy * deltaTime,
                            this->spriteExplosion);
                        //enable lifetime destuction:
                        explosion.enableLifeTime = true;
                        explosion.lifeTime = 0.15f;
                        //dont detect collision with this object
                        explosion.enableCollision = false;

                        //add to list
                        listObjects.push_back(explosion);
                    }
                }
                else
                {
                    object.remove = true;
                }
            }

            // Can object be seen?
            float vecX = object.x - playerX;
            float vecY = object.y - playerY;
            float distanceFromPlayer = sqrtf(vecX * vecX + vecY * vecY);

            float eyeX = cosf(playerAngle);
            float eyeY = sinf(playerAngle);
            float objectAngle = atan2f(vecY, vecX) - atan2f(eyeY, eyeX);

            //限制取值范围在+PI与-PI之间
            if (objectAngle < -3.14159f)
                objectAngle += 2.0f * 3.14159f;
            if (objectAngle > 3.14159f)
                objectAngle -= 2.0f * 3.14159f;

            bool inPlayerFOV = fabs(objectAngle) < FOV / 2.0f;

            //画在视野范围之内但是不要太近的物体, 不画超过视距的物体
            if (inPlayerFOV && distanceFromPlayer >= 0.5f && distanceFromPlayer < depth)
            {
                float objectCeiling = (float)(ScreenHeight() / 2.0) - ScreenHeight() / ((float)distanceFromPlayer);
                float objectFloor = ScreenHeight() - objectCeiling;
                float objectHeight = objectFloor - objectCeiling;

                float objectAspectRatio = (float)object.sprite->nHeight / (float)object.sprite->nWidth;
                float objectWidth = objectHeight / objectAspectRatio;
                float middleOfObject = (0.5f * (objectAngle / (FOV / 2.0f)) + 0.5f) * (float)ScreenWidth();

                // Draw Lamp:
                for (float lx = 0; lx < objectWidth; lx++)
                {
                    for (float ly = 0; ly < objectHeight; ly++)
                    {
                        float sampleX = lx / objectWidth;
                        float sampleY = ly / objectHeight;
                        wchar_t c = object.sprite->SampleGlyph(sampleX, sampleY);

                        int nObjectColumn = (int)(middleOfObject + lx - (objectWidth / 2.0f));

                        if (nObjectColumn >= 0 && nObjectColumn < ScreenWidth())
                        {
                            //enable transparency :)
                            //we only draw stuffs front of walls:
                            if (c != L' ' && fDepthBuffer[nObjectColumn] >= distanceFromPlayer)
                            {
                                short att = object.sprite->SampleColour(sampleX, sampleY);

                                ConsoleColor foreColor = (ConsoleColor)(att & 0x000F);
                                ConsoleColor backColor = (ConsoleColor)((att & 0x00F0) / 16);

                                Color24 pixelColor = palette[foreColor];
                                UNUSED(backColor);

                                Draw(nObjectColumn, (int)(objectCeiling + ly),
                                    Pixel(pixelColor.r, pixelColor.g, pixelColor.b));

                                //update depth buffer(simple fix)
                                this->fDepthBuffer[nObjectColumn] = distanceFromPlayer;
                            }
                        }
                    }
                }
            }
        }

        //GC for fireball:
        listObjects.remove_if([](sObject& o)
            {
                return o.remove;
            });

        //draw map
        for (int y = 0; y < mapHeight; y++)
        {
            for (int x = 0; x < mapWidth; x++)
            {
                if (map[y * mapWidth + x] == L'#')
                {
                    Draw(x, y, Pixel(180, 0, 40));
                }
                else
                {
                    Draw(x, y, Pixel(33, 212, 33));
                }
            }
        }
        //draw player
        Draw((int)playerX, (int)playerY, Pixel(255, 255, 255));

        return true;
    }

    // Called once on application termination, so you can be one clean coder
    bool OnUserDestroy() override
    {
        delete this->spriteWall;
        delete this->spriteLamp;
        delete this->spriteFireBall;
        delete this->spriteExplosion;

        delete this->bgm;
        delete this->bgm2;
        delete this->explosionSound;
        delete this->fireBallSound;
        delete this->explosionPool;
        delete this->fireBallPool;

        delete[] this->fDepthBuffer;

        return true;
    }
};

void debug_output_line(const wstring& info)
{
    OutputDebugString((info + L"\n").c_str());
}

//editor for olcSprite and Color32!
class PixelEditor : public PixelGameEngine
{
public:
    wstring spritePath;
    int initialSpriteSizeX = 32;
    int initialSpriteSizeY = 32;

private:
    //palette:
    std::map<ConsoleColor, Color24> palette;

    olcSprite* spritePtr;
    int spriteSizeX;
    int spriteSizeY;

    int defaultSpritePosX = 20;
    int defaultSpritePosY = 20;

    int spritePosX = defaultSpritePosX;
    int spritePosY = defaultSpritePosY;
    int defaultZoomPixelScaler = 1;

    //palette setting props:
    //-1 means you didn't choose any color yet.
    int choosenPaletteColorIndex = -1;

    int paletteColorSizeX = 10;
    int paletteColorSizeY = 10;

    int prevMousePosX = 0;
    int prevMousePosY = 0;

    //save setting:
    bool isDirty = false;

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
    PixelEditor()
    {
        sAppName = "PixelEditor";
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
        spriteSizeX = spritePtr->nWidth;
        spriteSizeY = spritePtr->nHeight;

        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override
    {
        int mw = GetMouseWheel();

        //debug_output_line(to_wstring(mw));

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
            this->choosenPaletteColorIndex = -1;
        }

        //save:
        if (GetKey(Key::S).bPressed)
        {
            spritePtr->Save(spritePath);
            isDirty = false;
        }

        int mouseX = GetMouseX();
        int mouseY = GetMouseY();
        int heldLeftMouseButton = GetMouse(Mouse::RIGHT).bHeld;

        //debug_output_line(to_wstring(mouseX) + L" " + to_wstring(mouseY));

        //choose color:
        if (mouseX >= 0 && mouseX < paletteColorSizeX &&
            mouseY >= 0 && mouseY < paletteColorSizeY * palette.size())
        {
            if (GetMouse(Mouse::LEFT).bHeld)
            {
                this->choosenPaletteColorIndex = mouseY / paletteColorSizeY;
            }
        }

        if (mouseX >= spritePosX && mouseY >= spritePosY &&
            mouseX < spritePosX + spriteSizeX * defaultZoomPixelScaler &&
            mouseY < spritePosY + spriteSizeY * defaultZoomPixelScaler)
        {
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

            //debug_output_line(to_wstring(mouseX) + L" " + to_wstring(mouseY) + L" " + to_wstring(heldLeftMouseButton) + L" in area!");

            //select pixel(drawing):
            if (GetMouse(Mouse::LEFT).bHeld)
            {
                int mouseInSpritePosX = mouseX - spritePosX;
                int mouseInSpritePosY = mouseY - spritePosY;

                int selectIndexX = mouseInSpritePosX / defaultZoomPixelScaler;
                int selectIndexY = mouseInSpritePosY / defaultZoomPixelScaler;

                //debug_output_line(to_wstring(selectIndexX) + L" " + to_wstring(selectIndexY));

                //draw alpha(erase):
                if (this->choosenPaletteColorIndex == -1)
                {
                    //set it to 0:
                    this->spritePtr->SetColour(selectIndexX, selectIndexY, 0);
                    //disable draw this pixel:
                    this->spritePtr->SetGlyph(selectIndexX, selectIndexY, ' ');
                }
                else
                {
                    ushort att = ToUshort((ConsoleColor)this->choosenPaletteColorIndex, ConsoleColor::BLACK);
                    this->spritePtr->SetColour(selectIndexX, selectIndexY, att);
                    //fill
                    this->spritePtr->SetGlyph(selectIndexX, selectIndexY, 'a');
                }

                //set dirty sign:
                isDirty = true;
            }
        }
        else
        {
            prevMousePosX = 0;
            prevMousePosY = 0;

            //debug_output_line(to_wstring(mouseX) + L" " + to_wstring(mouseY)+ L" " + to_wstring(heldLeftMouseButton) + L" not in area.");
        }

        //clear screen:
        this->Clear(Pixel(44, 44, 44));

        vi2d textSize = GetTextSize("Welcome to Pixel Editor");
        vi2d text2Size = GetTextSize("Choosen Color:");

        //draw texts:
        DrawString({ 50, 0 }, "Welcome to Pixel Editor");
        DrawString({ 50, textSize.y }, "Choosen Color:");

        //draw dirty sign:
        if (isDirty)
        {
            DrawString({ 50 + textSize.x,0 }, "*");
        }

        //draw sprite size:
        DrawString({ 50, textSize.y * 2 }, "Size:" + to_string(spriteSizeX) + "X" + to_string(spriteSizeY));

        //draw choosen color:
        if (this->choosenPaletteColorIndex != -1)
        {
            for (int y = 0; y < paletteColorSizeY; y++)
            {
                for (int x = 0; x < paletteColorSizeX; x++)
                {
                    Color24 color = palette[(ConsoleColor)this->choosenPaletteColorIndex];
                    Draw({ x + 50 + text2Size.x, y + text2Size.y }, Pixel(color.r, color.g, color.b));
                }
            }
        }
        //choosen is erase:
        else
        {
            DrawString({ 50 + text2Size.x, text2Size.y }, "erase", Pixel(125, 0, 66));
        }

        //draw sprite bg(ex info for painting):
        for (int i = 0; i < spriteSizeY * defaultZoomPixelScaler; i++)
        {
            for (int j = 0; j < spriteSizeX * defaultZoomPixelScaler; j++)
            {
                Draw(j + spritePosX, i + spritePosY, Pixel(88, 88, 88));
            }
        }

        //draw sprite:
        DisplaySprite(spritePtr, spritePosX, spritePosY, defaultZoomPixelScaler);

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

        return true;
    }
};

//#define CMD_EDITOR_CONFIG

int main(int argc, char** argv)
{

#if defined(CMD_EDITOR_CONFIG)
    //try to open editor
    if (argc > 1)
    {
        const char* cmd1 = argv[1];
        //bool eqauls = strcmpi(cmd1, "") == 0;

        PixelEditor editor;

        if (editor.Construct(320, 180, 4, 4))
            editor.Start();
    }
    else
    {
        PixelFPSDemo game;

        if (game.Construct(320, 180, 4, 4))
            game.Start();
    }

    return 0;

#else

    char input = '\0';
    cout << "welcome to PixelFPSDemo!\n";
    cout << "Press 'E' open Editor, Press any other key open Game.\n";
    cin >> input;

    if (tolower(input) == 'e')
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

            //create new file
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
            }
            else
            {
                finalFilePath = folderPath + fileNames[chooseIndex - 1];
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
        PixelFPSDemo game;

        if (game.Construct(320, 180, 4, 4))
            game.Start();
    }

    return 0;

#endif
}
