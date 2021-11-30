#pragma once

#include "Navigation.hpp"
#include "GameObject.hpp"
#include "Component.hpp"
#include "Transform.hpp"

class PixelFPSDemo2 : public PixelGameEngine
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

    //obstacles:
    std::vector<Vector2> obstacles;
    //for AI:
    //NOTE, sObject and listObjects can't interact normally with Navigation system.
    //So, im writing a new system.
    sObject* simple_ai;
    float nav_timer = 0;
    bool enableNav = false;

private:
    //tool functions:
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

private:
    void update_audio()
    {
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

    }

    void receive_user_input(float deltaTime)
    {
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

    }

    void render_world(float deltaTime)
    {
        //#define OLD_RAYCAST
#ifdef OLD_RAYCAST
        //raycast
        for (int x = 0; x < ScreenWidth(); x++)
        {
            float rayAngle = (playerAngle - FOV / 2.0f) + ((float)x / ScreenWidth()) * FOV;
            float diffAngle = playerAngle - rayAngle;

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

                //超出地图边界
                //out of map:
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

            //update depth buffer:
            fDepthBuffer[x] = distanceToWall;

            //note!!! * cosf(diffAngle) will cause noise on screen, but we fixed fisheye problem.
            //int ceiling = (int)(ScreenHeight() / 2.0f - ScreenHeight() / distanceToWall);
            int ceiling = (int)(ScreenHeight() / 2.0f - ScreenHeight() / (distanceToWall * cosf(diffAngle)));
            int floor = ScreenHeight() - ceiling;

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
#else
//raycast
        for (int x = 0; x < ScreenWidth(); x++)
        {
            float rayAngle = (playerAngle - FOV / 2.0f) + ((float)x / ScreenWidth()) * FOV;
            float diffAngle = playerAngle - rayAngle;

            const float stepSize = 0.01f;

            float distanceToWall = 0.0f;
            bool hitWall = false;

            float eyeX = ::cosf(rayAngle);
            float eyeY = ::sinf(rayAngle);

            float sampleX = 0.0f;

            //DDA raycast:
            HitInfo hitInfo;
            CheckFunc checkFunc = [](int x, int y, int width, int height, const std::wstring& map)
            {
                //超出地图边界
                //out of map:
                if (x < 0 || x >= width || y < 0 || y >= height)
                {
                    return true;
                }
                //与墙壁发生了碰撞
                //collsion detect
                else if (map[y * width + x] == L'#')
                {
                    return true;
                }
                return false;
            };
            if (raycastDDA({ playerX, playerY }, { eyeX, eyeY }, checkFunc, hitInfo))
            {
                distanceToWall = (hitInfo.hitPos - vf2d(playerX, playerY)).mag();
                sampleX = hitInfo.sampleX;
            }
            else
            {
                throw "?";
            }

            //update depth buffer:
            fDepthBuffer[x] = distanceToWall;

            //note!!! * cosf(diffAngle) will cause noise on screen, but we fixed fisheye problem.
            //int ceiling = (int)(ScreenHeight() / 2.0f - ScreenHeight() / distanceToWall);
            int ceiling = (int)(ScreenHeight() / 2.0f - ScreenHeight() / (distanceToWall * cosf(diffAngle)));
            int floor = ScreenHeight() - ceiling;

            for (int y = 0; y < ScreenHeight(); y++)
            {
                //draw ceiling
                if (y <= ceiling)
                {
                    float planeZ = (ScreenHeight() / 2.0f) / ((ScreenHeight() / 2.0f) - y);
                    vf2d planePoint = vf2d(playerX, playerY) + vf2d(eyeX, eyeY) * planeZ * 2.0f / cos(diffAngle);

                    // Work out which planar tile we are in
                    int planeTileX = int(planePoint.x);
                    int planeTileY = int(planePoint.y);

                    // Work out normalised offset into planar tile
                    float planeSampleX = planePoint.x - planeTileX;
                    float planeSampleY = planePoint.y - planeTileY;

                    Pixel pixel = shade(planeTileX, planeTileY, CellSide::Top, Color24(0, 0, 255), planeSampleX, planeSampleY, planeZ);
                    //Pixel pixel = shadeFloorAndCeiling(Color24(0, 0, 255));
                    Draw(x, y, pixel);
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

                        //Draw(x, y, Pixel(pixelColor.r, pixelColor.g, pixelColor.b));
                        Pixel pixel = shade(hitInfo.hitMapPos.x, hitInfo.hitMapPos.y, hitInfo.side, pixelColor, sampleX, sampleY, distanceToWall);
                        Draw(x, y, pixel);
                    }
                    else
                    {
                        Draw(x, y, Pixel(0, 0, 0));
                    }
                }
                //draw floor
                else
                {
                    float planeZ = (ScreenHeight() / 2.0f) / (y - (ScreenHeight() / 2.0f));
                    vf2d planePoint = vf2d(playerX, playerY) + vf2d(eyeX, eyeY) * planeZ * 2.0f / cos(diffAngle);

                    // Work out which planar tile we are in
                    int planeTileX = int(planePoint.x);
                    int planeTileY = int(planePoint.y);

                    // Work out normalised offset into planar tile
                    float planeSampleX = planePoint.x - planeTileX;
                    float planeSampleY = planePoint.y - planeTileY;

                    Pixel pixel = shade(planeTileX, planeTileY, CellSide::Bottom, Color24(0, 128, 0), planeSampleX, planeSampleY, planeZ);
                    //Pixel pixel = shadeFloorAndCeiling(Color24(0, 128, 0));
                    Draw(x, y, pixel);
                }
            }
        }
#endif

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

    }

    typedef bool (*CheckFunc)(int x, int y, int width, int height, const std::wstring& map);

    // Identifies side of cell
    enum class CellSide
    {
        North,
        East,
        South,
        West,
        Top,
        Bottom,
    };

    struct HitInfo
    {
    public:
        vf2d hitPos;
        vi2d hitMapPos;
        float sampleX;
        CellSide side;
    };

    bool raycastDDA(const vf2d& origin, const vf2d& dir, CheckFunc checkFunc, HitInfo& hitInfo)
    {
        vf2d rayDelta = {
            sqrtf(1 + powf(dir.y / dir.x, 2)),
            sqrtf(1 + powf(dir.x / dir.y, 2)) };

        vi2d mapPos = origin;
        vi2d step;
        vf2d rayLength;

        if (dir.x < 0)
        {
            step.x = -1;
            rayLength.x = (origin.x - mapPos.x) * rayDelta.x;
        }
        else
        {
            step.x = 1;
            rayLength.x = (mapPos.x + 1 - origin.x) * rayDelta.x;
        }

        if (dir.y < 0)
        {
            step.y = -1;
            rayLength.y = (origin.y - mapPos.y) * rayDelta.y;
        }
        else
        {
            step.y = 1;
            rayLength.y = (mapPos.y + 1 - origin.y) * rayDelta.y;
        }

        olc::vf2d intersection;
        float maxDistance = 100.0f; //just a random large value
        float distance = 0.0f;

        while (distance < maxDistance)
        {
            if (rayLength.x < rayLength.y)
            {
                rayLength.x += rayDelta.x;
                mapPos.x += step.x;
            }
            else
            {
                rayLength.y += rayDelta.y;
                mapPos.y += step.y;
            }

            distance = vf2d((float)mapPos.x - origin.x, (float)mapPos.y - origin.y).mag();

            //hit wall
            if (checkFunc(mapPos.x, mapPos.y, mapWidth, mapHeight, map))
            {
                hitInfo.hitMapPos = mapPos;

                // Find accurate Hit Location
                float m = dir.y / dir.x;

                // From Top Left
                if (origin.y <= mapPos.y)
                {
                    if (origin.x <= mapPos.x)
                    {
                        hitInfo.side = CellSide::West;
                        intersection.y = m * (mapPos.x - origin.x) + origin.y;
                        intersection.x = float(mapPos.x);
                        hitInfo.sampleX = intersection.y - std::floor(intersection.y);
                    }
                    else if (origin.x >= (mapPos.x + 1))
                    {
                        hitInfo.side = CellSide::East;
                        intersection.y = m * ((mapPos.x + 1) - origin.x) + origin.y;
                        intersection.x = float(mapPos.x + 1);
                        hitInfo.sampleX = intersection.y - std::floor(intersection.y);
                    }
                    else
                    {
                        hitInfo.side = CellSide::North;
                        intersection.y = float(mapPos.y);
                        intersection.x = (mapPos.y - origin.y) / m + origin.x;
                        hitInfo.sampleX = intersection.x - std::floor(intersection.x);
                    }

                    if (intersection.y < mapPos.y)
                    {
                        hitInfo.side = CellSide::North;
                        intersection.y = float(mapPos.y);
                        intersection.x = (mapPos.y - origin.y) / m + origin.x;
                        hitInfo.sampleX = intersection.x - std::floor(intersection.x);
                    }
                }
                else if (origin.y >= mapPos.y + 1)
                {
                    if (origin.x <= mapPos.x)
                    {
                        hitInfo.side = CellSide::West;
                        intersection.y = m * (mapPos.x - origin.x) + origin.y;
                        intersection.x = float(mapPos.x);
                        hitInfo.sampleX = intersection.y - std::floor(intersection.y);
                    }
                    else if (origin.x >= (mapPos.x + 1))
                    {
                        hitInfo.side = CellSide::East;
                        intersection.y = m * ((mapPos.x + 1) - origin.x) + origin.y;
                        intersection.x = float(mapPos.x + 1);
                        hitInfo.sampleX = intersection.y - std::floor(intersection.y);
                    }
                    else
                    {
                        hitInfo.side = CellSide::South;
                        intersection.y = float(mapPos.y + 1);
                        intersection.x = ((mapPos.y + 1) - origin.y) / m + origin.x;
                        hitInfo.sampleX = intersection.x - std::floor(intersection.x);
                    }

                    if (intersection.y > (mapPos.y + 1))
                    {
                        hitInfo.side = CellSide::South;
                        intersection.y = float(mapPos.y + 1);
                        intersection.x = ((mapPos.y + 1) - origin.y) / m + origin.x;
                        hitInfo.sampleX = intersection.x - std::floor(intersection.x);
                    }
                }
                else
                {
                    if (origin.x <= mapPos.x)
                    {
                        hitInfo.side = CellSide::West;
                        intersection.y = m * (mapPos.x - origin.x) + origin.y;
                        intersection.x = float(mapPos.x);
                        hitInfo.sampleX = intersection.y - std::floor(intersection.y);
                    }
                    else if (origin.x >= (mapPos.x + 1))
                    {
                        hitInfo.side = CellSide::East;
                        intersection.y = m * ((mapPos.x + 1) - origin.x) + origin.y;
                        intersection.x = float(mapPos.x + 1);
                        hitInfo.sampleX = intersection.y - std::floor(intersection.y);
                    }
                }

                hitInfo.hitPos = intersection;

                return true;
            }
        }

        return false;
    }

    //you can add shader code here:
    Pixel shade(int mapPosX, int mapPosY, CellSide side, Color24 pixelColor, float sampleX, float sampleY, float distance)
    {
        Pixel pixel(pixelColor.r, pixelColor.g, pixelColor.b);

        float shadow = 1.0f;

        ////point light:
        //switch (side)
        //{
        //case CellSide::Top:
        //    pixel.r = pixel.r * 0.5f;
        //    pixel.g = pixel.g * 0.5f;
        //    pixel.b = pixel.b * 0.5f;
        //    break;
        //default:
        //    vf2d pixelPos = vf2d(mapPosX + sampleX, mapPosY + sampleY);
        //    vf2d playerPos(playerX, playerY);
        //    float distanceToPlayer = (playerPos - pixelPos).mag();
        //    if (distanceToPlayer > 5)
        //    {
        //        pixel.r = pixel.r * 0.5f;
        //        pixel.g = pixel.g * 0.5f;
        //        pixel.b = pixel.b * 0.5f;
        //    }
        //    break;
        //}

        //based on distance between player and pixel:
        //vf2d pixelPos = vf2d(mapPosX + sampleX, mapPosY + sampleY);
        //vf2d flowerPos(playerX, playerY);
        //float distanceToFlower = (flowerPos - pixelPos).mag();
        //float flowerLight = max(0.2f, 1.0f - min(distanceToFlower / 5, 1.0f));
        //pixel.r = pixel.r * flowerLight;
        //pixel.g = pixel.g * flowerLight;
        //pixel.b = pixel.b * flowerLight;

        //directional light and distance:
        //float fDistance = 1.0f;
        //if (distance > 5)
        //{
        //    //fDistance = 1.0f - std::min(distance / depth, 1.0f);
        //}
        //pixel.r = pixel.r * fDistance * shadow;
        //pixel.g = pixel.g * fDistance * shadow;
        //pixel.b = pixel.b * fDistance * shadow;

        switch (side)
        {
        case CellSide::Bottom:
        case CellSide::Top:
            pixel.r = pixel.r * 0.25f;
            pixel.g = pixel.g * 0.25f;
            pixel.b = pixel.b * 0.25f;
            break;
        }

        //fog:
        Color24 fogColor(192, 192, 192);
        float fDistance = 1.0f;
        fDistance = 1.0f - std::min(distance / 15, 1.0f);
        float fog = 1.0 - fDistance;
        pixel.r = fDistance * pixel.r + fog * fogColor.r;
        pixel.g = fDistance * pixel.g + fog * fogColor.g;
        pixel.b = fDistance * pixel.b + fog * fogColor.b;

        //distance:
        float _d = 1.0f;
        _d = 1.0f - std::min(distance / depth, 0.4f);
        pixel.r = pixel.r * _d;
        pixel.g = pixel.g * _d;
        pixel.b = pixel.b * _d;

        return pixel;
    }

    //legacy function, use shade instead.
    Pixel shadeFloorAndCeiling(Color24 pixelColor)
    {
        Pixel pixel(pixelColor.r, pixelColor.g, pixelColor.b);

        //float intensity = 0.3f;
        float intensity = 1.0f;

        pixel.r = pixel.r * intensity;
        pixel.g = pixel.g * intensity;
        pixel.b = pixel.b * intensity;
        return pixel;
    }

public:
    PixelFPSDemo2()
    {
        this->sAppName = "PixelFPS Demo2";
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
            //sObject(11.5f, 6.5f, this->spriteFlower),
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
        for (size_t i = 0; i < ScreenWidth(); i++)
        {
            this->fDepthBuffer[i] = INFINITY;
        }

        //this->bgm = new Audio(L"../../res/audios/[CSO] Zombie Scenario - Normal Fight.mp3");
        //this->bgm2 = new Audio(L"../../res/audios/[CSO] Zombie Scenario - Round Start.mp3");
        this->bgm = new Audio(L"../../res/audios/Silent Hill 2 OST - Laura Plays The Piano.mp3");
        this->bgm2 = new Audio(L"../../res/audios/Silent Hill 2 OST - True.mp3");
        this->explosionSound = new Audio(L"../../res/audios/548_Effect.Explosion.wav.mp3");
        this->fireBallSound = new Audio(L"../../res/audios/560_Weapon.Rocket.Fire.wav.mp3");

        this->explosionPool = new AudioPool(L"../../res/audios/548_Effect.Explosion.wav.mp3");
        this->fireBallPool = new AudioPool(L"../../res/audios/560_Weapon.Rocket.Fire.wav.mp3");

        this->bgm2->SetVolume(MCI_MAX_VOLUME / 3);
        this->bgm2->Play(false, false);

        //set obstacles:
        if (this->enableNav)
        {
            for (int y = 0; y < mapHeight; y++)
            {
                for (int x = 0; x < mapWidth; x++)
                {
                    if (this->map[y * mapWidth + x] == L'#')
                    {
                        obstacles.push_back({ x, y });
                    }
                }
            }
            simple_ai = new sObject(2, 2, this->spriteFlower);
        }
        return true;
    }

    // Called every frame, and provides you with a time per frame value
    bool OnUserUpdate(float fElapsedTime) override
    {
        float deltaTime = fElapsedTime;

        //ai nav:
        if (enableNav)
        {
            nav_timer += deltaTime;
            if (nav_timer >= 1.0f)
            {
                nav_timer = 0;
                auto sr = Navigation::Navigate({ (int)simple_ai->x, (int)simple_ai->y }, { (int)playerX, (int)playerY }, SearchDirection::Eight, 100, obstacles, SearchMethod::DFS);
                if (sr.success)
                {
                    auto tar_vec = sr.path[1].position - sr.path[0].position;
                    simple_ai->x += tar_vec.x;
                    simple_ai->y += tar_vec.y;
                    debug_output_vector2(::vi2d(simple_ai->x, simple_ai->y));
                }
            }
        }

        if (GetKey(Key::ESCAPE).bPressed)
        {
            return false;
        }

        update_audio();

        receive_user_input(deltaTime);

        render_world(deltaTime);

        //GC for fireball:
        listObjects.remove_if([](sObject& o)
            {
                return o.remove;
            });

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