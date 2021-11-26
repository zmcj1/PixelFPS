//  PixelFPS
//  author : zmcj1
//  date : 2021 / 11 / 26
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

struct Color32
{
public:
    ::byte r;
    ::byte g;
    ::byte b;
    ::byte a;

    Color32()
    {
        this->r = 0;
        this->g = 0;
        this->b = 0;
        this->a = 0;
    }

    Color32(::byte r, ::byte g, ::byte b, ::byte a)
    {
        this->r = r;
        this->g = g;
        this->b = b;
        this->a = a;
    }

    bool operator ==(const Color32& other) const
    {
        if (r == other.r && g == other.g && b == other.b && a == other.a)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    bool operator !=(const Color32& other) const
    {
        if (r == other.r && g == other.g && b == other.b && a == other.a)
        {
            return false;
        }
        else
        {
            return true;
        }
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

    //new function resize
    void Resize(int newWidth, int newHeight)
    {
        short* newGlyphs = new short[newWidth * newHeight];
        short* newColours = new short[newWidth * newHeight];

        for (int y = 0; y < newHeight; y++)
        {
            for (int x = 0; x < newWidth; x++)
            {
                newGlyphs[y * newWidth + x] = this->GetGlyph(x, y);
                newColours[y * newWidth + x] = this->GetColour(x, y);
            }
        }

        delete[] this->m_Glyphs;
        delete[] this->m_Colours;

        this->m_Glyphs = newGlyphs;
        this->m_Colours = newColours;

        this->nWidth = newWidth;
        this->nHeight = newHeight;
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
    olcSprite* spriteFlower;

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
        this->spriteFlower = new olcSprite(L"../../res/flower.spr");

        listObjects =
        {
            sObject(8.5f, 8.5f, this->spriteLamp),
            sObject(7.5f, 7.5f, this->spriteLamp),
            sObject(10.5f, 3.5f, this->spriteLamp),
            sObject(11.5f, 6.5f, this->spriteFlower),
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
                //collsion detect
                else if (map[testY * mapWidth + testX] == L'#')
                {
                    hitWall = true;

                    //墙壁中心点(假设每一堵墙壁都是1X1大小)
                    //suppose wall' size is 1X1
                    float blockMidX = (float)testX + 0.5f;
                    float blockMidY = (float)testY + 0.5f;
                    //射线命中点
                    //hit point position
                    float testPointX = playerX + eyeX * distanceToWall;
                    float testPointY = playerY + eyeY * distanceToWall;

                    //testAngle为从block中点出发, 射向射线命中点的向量与X轴的夹角
                    //angle between vector and x axis
                    float testAngle = atan2f(testPointY - blockMidY, testPointX - blockMidX);

                    //命中右方
                    //hit right side
                    if (testAngle >= -3.14159f * 0.25f && testAngle < 3.14159f * 0.25f)
                        sampleX = testPointY - (float)testY;
                    //命中上方
                    //hit top side
                    if (testAngle >= 3.14159f * 0.25f && testAngle < 3.14159f * 0.75f)
                        sampleX = testPointX - (float)testX;
                    //命中下方
                    //hit bottom side
                    if (testAngle < -3.14159f * 0.25f && testAngle >= -3.14159f * 0.75f)
                        sampleX = testPointX - (float)testX;
                    //命中左方
                    //hit left side
                    if (testAngle >= 3.14159f * 0.75f || testAngle < -3.14159f * 0.75f)
                        sampleX = testPointY - (float)testY;
                }
            }

            int ceiling = (int)(ScreenHeight() / 2.0f - ScreenHeight() / distanceToWall);
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

        //update objects(render object & update physics):
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
                // Check if object is inside wall - set flag for removing
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

                    //collsion with other objects:
                    //todo:
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
            //set limit : [-PI, PI]
            if (objectAngle < -3.14159f)
                objectAngle += 2.0f * 3.14159f;
            if (objectAngle > 3.14159f)
                objectAngle -= 2.0f * 3.14159f;

            bool inPlayerFOV = fabs(objectAngle) < FOV / 2.0f;

            //画在视野范围之内但是不要太近的物体, 不画超过视距的物体
            //draw object witch is in FOV
            if (inPlayerFOV && distanceFromPlayer >= 0.5f && distanceFromPlayer < depth)
            {
                float objectCeiling = (float)(ScreenHeight() / 2.0) - ScreenHeight() / ((float)distanceFromPlayer);
                float objectFloor = ScreenHeight() - objectCeiling;
                float objectHeight = objectFloor - objectCeiling;

                float objectAspectRatio = (float)object.sprite->nHeight / (float)object.sprite->nWidth;
                float objectWidth = objectHeight / objectAspectRatio;
                float middleOfObject = (objectAngle / FOV + 0.5f) * (float)ScreenWidth();

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
        delete this->spriteFlower;

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

//writeline:
void debug_output_vector2(const vi2d& vec2)
{
    debug_output_line(to_wstring(vec2.x) + L" " + to_wstring(vec2.y));
}

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

                line += L"(" + to_wstring(saveColor.r) + L"," + to_wstring(saveColor.g) + L"," + to_wstring(saveColor.b) + L"," + to_wstring(saveColor.a) + L"),";
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

        return true;
    }
};

int main()
{
    char input_char = '\0';
    cout << "welcome to PixelFPSDemo!\n";
    cout << "Press 'E' open Editor, Press any other key open Game.\n";
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
        PixelFPSDemo game;

        if (game.Construct(320, 180, 4, 4))
            game.Start();
    }

    return 0;
}
