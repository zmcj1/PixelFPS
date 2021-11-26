﻿#pragma once

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