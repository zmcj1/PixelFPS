﻿#pragma once

#include "Navigation.hpp"
#include "GameObject.hpp"
#include "Component.hpp"
#include "Transform.hpp"
#include "Collider.hpp"
#include "SpriteRenderer.hpp"
#include "LifeController.hpp"
#include "GameManager.hpp"
#include "Weapon.hpp"
#include "Input.hpp" //mouse support
#include "Resources.hpp" //load
#include <unordered_map>

class PixelFPSDemo2 : public PixelGameEngine
{
private:
    //graphics setting:
    bool useOldRaycastObject = true;

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

    // enable mouse rotate:
    bool enableMouse = false;
    float mouseSpeed = 0.05f;

    //gameplay:
    const int fullHealth = 100;
    int selfDamage = 1;
    int playerHealth = fullHealth;

    //weapons:
    WeaponEnum weapon_current = WeaponEnum::DESERT_EAGLE;
    unordered_map<int, Weapon*> weapons;

    //hud
    bool enableHud = true;
    bool enableBloodBar = false;
    float weapon_Ypos = 1.0f;
    float weapon_Xcof = 1.0f;
    bool bobbing_side = false;

    //muzzle flame:
    bool enableFlame = false;
    const float flameInterval = 0.1f;
    float flameTimer = 0.0f;

    //camera:
    const float FOV = 3.14159f / 4; // Field of view
    const float depth = 32.0f;      // Maximum rendering distance

    //sprites:
    OLCSprite* spriteWall;
    OLCSprite* spriteLamp;
    OLCSprite* spriteFireBall;
    OLCSprite* spriteExplosion;
    OLCSprite* spriteFlower;
    OLCSprite* sptireWeapon_aek;
    OLCSprite* spriteBullet;

    //new weapons:
    OLCSprite* spriteDesertEagle;
    OLCSprite* spriteAK47;
    OLCSprite* spriteM4A1;

    //GameManager:
    GameManager& GM;

    //palette:
    std::map<ConsoleColor, Color24> palette;

    //A depth buffer used to sort pixels in Z-Axis:
    float* depthBuffer = nullptr;

    //audios:
    Audio* bgm = nullptr;
    Audio* bgm2 = nullptr;
    bool startedPlayBGM = false;

    //sounds:    
    Audio* explosionSound = nullptr;
    Audio* fireBallSound = nullptr;
    AudioPool* explosionPool = nullptr;
    AudioPool* fireBallPool = nullptr;
    AudioPool* aekBulletPool = nullptr;

    //new sounds:
    AudioPool* dePool = nullptr;
    AudioPool* ak47Pool = nullptr;
    AudioPool* m4a1Pool = nullptr;

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
        OLCSprite sprite(path);
        DisplaySprite(&sprite, x, y, zoomPixelScaler);
    }

    void DisplaySprite(OLCSprite* sprite, int x, int y, int zoomPixelScaler)
    {
        //draw with scaler:
        for (size_t i = 0; i < sprite->Height * zoomPixelScaler; i++)
        {
            for (size_t j = 0; j < sprite->Width * zoomPixelScaler; j++)
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

        //clean all weapon audio pool:
        aekBulletPool->Clean();
        dePool->Clean();
        ak47Pool->Clean();
        m4a1Pool->Clean();
    }

    void clamp_mouse_in_client()
    {
        POINT mousePosInClient = window.GetMappedMousePos();
        POINT clientSize = window.GetClientSize();
        POINT windowCenterPos = window.GetCenterPosOfWindow();

        const int width = 50;

        RECT rect;
        ::GetWindowRect(window.windowHandle, &rect);
        rect.left += width;
        rect.right -= width;
        rect.top = windowCenterPos.y;
        rect.bottom = windowCenterPos.y;
        ::ClipCursor(&rect);

        if (mousePosInClient.x < width * 2)
        {
            ::SetCursorPos(windowCenterPos.x, windowCenterPos.y);
            Input::ResetMouseAxis();
        }
        else if (mousePosInClient.x > clientSize.x - width * 2)
        {
            ::SetCursorPos(windowCenterPos.x, windowCenterPos.y);
            Input::ResetMouseAxis();
        }
    }

    void receive_user_input(float deltaTime)
    {
        if (!enableMouse)
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

                if (bobbing_side)
                {
                    weapon_Xcof += 0.3f;
                    if (weapon_Xcof >= 3.5f)
                    {
                        bobbing_side = false;
                    }
                }
                else
                {
                    weapon_Xcof -= 0.3f;
                    if (weapon_Xcof <= -3.5f)
                    {
                        bobbing_side = true;
                    }
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
        }
        else
        {
            Input::CheckMouseAxis();
            int diffX = Input::GetMouseAxis(MouseAxis::MOUSE_X);

            //clamp:
            HWND foreWindow = GetForegroundWindow();
            if (foreWindow == this->__gameWindow)
            {
                clamp_mouse_in_client();
            }

            //rotate:
            playerAngle += diffX * rotateSpeed * deltaTime * mouseSpeed;

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

                if (bobbing_side)
                {
                    weapon_Xcof += 0.3f;
                    if (weapon_Xcof >= 3.5f)
                    {
                        bobbing_side = false;
                    }
                }
                else
                {
                    weapon_Xcof -= 0.3f;
                    if (weapon_Xcof <= -3.5f)
                    {
                        bobbing_side = true;
                    }
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
            if (GetKey(Key::A).bHeld)
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
            if (GetKey(Key::D).bHeld)
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
        }

        //self Damage
        if (GetKey(Key::K).bHeld)
        {
            playerHealth -= selfDamage;
        }

        //switch weapon:
        if (GetKey(Key::K1).bPressed)
        {
            weapon_current = WeaponEnum::DESERT_EAGLE;
        }
        if (GetKey(Key::K2).bPressed)
        {
            weapon_current = WeaponEnum::AK47;
        }
        if (GetKey(Key::K3).bPressed)
        {
            weapon_current = WeaponEnum::AEK_971;
        }
        if (GetKey(Key::K4).bPressed)
        {
            weapon_current = WeaponEnum::M4A1;
        }

        //fire:
        Weapon* weapon = weapons[(int)weapon_current];
        //update timer:
        weapon->UpdateWeaponTimer(deltaTime);

        //update muzzle flame:
        if (enableFlame)
        {
            flameTimer += deltaTime;
            if (flameTimer >= flameInterval)
            {
                flameTimer = 0.0f;
                enableFlame = false;
            }
        }

        //user input:
        if (!enableMouse && GetKey(Key::SPACE).bHeld || enableMouse && GetMouse(Mouse::LEFT).bHeld)
        {
            if (weapon->CanFire())
            {
                GameObject* bullet = new GameObject();

                //set position:
                bullet->transform->position = vf2d(playerX, playerY);

                //make noise:
                float fNoise = (((float)rand() / (float)RAND_MAX) - 0.5f) * 0.1f;
                float speed = 20;
                float vx = cosf(playerAngle + fNoise) * speed;
                float vy = sinf(playerAngle + fNoise) * speed;

                //set velocity:
                bullet->transform->velocity = vf2d(vx, vy);

                //add sprite:
                SpriteRenderer* renderer = bullet->AddComponent<SpriteRenderer>();
                renderer->sprite = this->spriteBullet;

                //add collider:
                bullet->AddComponent<Collider>();

                if (weapon->weapon_enum == WeaponEnum::DESERT_EAGLE)
                {
                    //play fire sound:
                    dePool->PlayOneShot(0.5f);
                }
                if (weapon->weapon_enum == WeaponEnum::AK47)
                {
                    //play fire sound:
                    ak47Pool->PlayOneShot(0.5f);
                }
                if (weapon->weapon_enum == WeaponEnum::AEK_971)
                {
                    //play fire sound:
                    aekBulletPool->PlayOneShot(0.5f);
                }
                if (weapon->weapon_enum == WeaponEnum::M4A1)
                {
                    //play fire sound:
                    m4a1Pool->PlayOneShot(0.5f);
                }

                //set muzzle flame timer:
                if (!enableFlame)
                {
                    enableFlame = true;
                }
            }
        }
    }

    void DepthDraw(int x, int y, float z, olc::Pixel pixel)
    {
        if (z <= depthBuffer[y * ScreenWidth() + x])
        {
            Draw({ x, y }, pixel);
            depthBuffer[y * ScreenWidth() + x] = z;
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
        //raycast:
        for (size_t i = 0; i < ScreenWidth() * ScreenHeight(); i++)
        {
            this->depthBuffer[i] = INFINITY;
        }

        for (int x = 0; x < ScreenWidth(); x++)
        {
            float rayAngle = (playerAngle - FOV / 2.0f) + ((float)x / ScreenWidth()) * FOV;
            float diffAngle = playerAngle - rayAngle;

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
                    if (distanceToWall * cosf(diffAngle) < depth)
                    {
                        float sampleY = ((float)y - (float)ceiling) / ((float)floor - (float)ceiling);

                        short att = spriteWall->SampleColour(sampleX, sampleY);

                        ConsoleColor foreColor = (ConsoleColor)(att & 0x000F);
                        ConsoleColor backColor = (ConsoleColor)((att & 0x00F0) / 16);

                        Color24 pixelColor = palette[foreColor];
                        UNUSED(backColor);

                        //Draw(x, y, Pixel(pixelColor.r, pixelColor.g, pixelColor.b));
                        Pixel pixel = shade(hitInfo.hitMapPos.x, hitInfo.hitMapPos.y, hitInfo.side, pixelColor, sampleX, sampleY, distanceToWall);
                        DepthDraw(x, y, distanceToWall * cosf(diffAngle), pixel);
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

        //update game objects(render & collision):
        for (auto& item : GM.gameObjects)
        {
            GameObject* go = item.second;

            if (!go->active) continue;
            if (go->remove) continue;

            //lifetime detect:
            LifeController* life = go->GetComponent<LifeController>();
            if (life != nullptr && life->enable)
            {
                life->lifeTime -= deltaTime;
                if (life->lifeTime <= 0)
                {
                    go->remove = true;
                    continue;
                }
            }

            //update physics:
            go->transform->position += go->transform->velocity * deltaTime;

            //collision detect:
            Collider* collider = go->GetComponent<Collider>();
            if (collider != nullptr && collider->enable)
            {
                // Check if object is inside wall - set flag for removing
                if (go->transform->position.x >= 0 && go->transform->position.x < mapWidth &&
                    go->transform->position.y >= 0 && go->transform->position.y < mapHeight)
                {
                    //collsion with walls:
                    if (map[(int)go->transform->position.y * mapWidth + (int)go->transform->position.x] == L'#')
                    {
                        go->remove = true;

                        //play explosion sound:
                        explosionPool->PlayOneShot();

                        //instantiate explosion:
                        //这里创建新的游戏物体会不会导致遍历出现问题？需要进行测试
                        GameObject* explosion = new GameObject();
                        explosion->transform->position =
                            go->transform->position - go->transform->velocity * deltaTime;

                        SpriteRenderer* renderer = explosion->AddComponent<SpriteRenderer>();
                        renderer->sprite = this->spriteExplosion;

                        LifeController* life = explosion->AddComponent<LifeController>();
                        life->lifeTime = 0.15f;
                    }

                    //collsion with other objects:
                    //todo:
                }
                else
                {
                    go->remove = true;
                }
            }

            //render:
            SpriteRenderer* renderer = go->GetComponent<SpriteRenderer>();
            if (renderer != nullptr && renderer->enable)
            {
                // Can object be seen?
                float vecX = go->transform->position.x - playerX;
                float vecY = go->transform->position.y - playerY;
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

                if (useOldRaycastObject)
                {
                    bool inPlayerFOV = fabs(objectAngle) < FOV / 2.0f;
                    //画在视野范围之内但是不要太近的物体, 不画超过视距的物体
                    //draw object witch is in FOV
                    if (inPlayerFOV && distanceFromPlayer >= 0.5f && distanceFromPlayer < depth)
                    {
                        float objectCeiling = ScreenHeight() / 2.0f - ScreenHeight() / distanceFromPlayer;
                        float objectFloor = ScreenHeight() - objectCeiling;
                        float objectHeight = objectFloor - objectCeiling;

                        float objectAspectRatio = (float)renderer->sprite->Height / (float)renderer->sprite->Width;
                        float objectWidth = objectHeight / objectAspectRatio;
                        float middleOfObject = (objectAngle / FOV + 0.5f) * (float)ScreenWidth();

                        // Draw Object:
                        for (float lx = 0; lx < objectWidth; lx++)
                        {
                            for (float ly = 0; ly < objectHeight; ly++)
                            {
                                float sampleX = lx / objectWidth;
                                float sampleY = ly / objectHeight;
                                wchar_t c = renderer->sprite->SampleGlyph(sampleX, sampleY);

                                int x = (int)(middleOfObject + lx - (objectWidth / 2.0f));
                                int y = (int)(objectCeiling + ly);

                                if (x >= 0 && x < ScreenWidth() && y >= 0 && y < ScreenHeight())
                                {
                                    //enable transparency :)
                                    if (c != L' ')
                                    {
                                        short att = renderer->sprite->SampleColour(sampleX, sampleY);

                                        ConsoleColor foreColor = (ConsoleColor)(att & 0x000F);
                                        ConsoleColor backColor = (ConsoleColor)((att & 0x00F0) / 16);

                                        Color24 pixelColor = palette[foreColor];
                                        UNUSED(backColor);

                                        //shade objects:
                                        Pixel pixel = shade_object(sampleX, sampleY, pixelColor, distanceFromPlayer);
                                        DepthDraw(x, y, distanceFromPlayer, pixel);
                                    }
                                }
                            }
                        }
                    }
                }
                else
                {
                    bool inPlayerFOV = fabs(objectAngle) < (FOV + 1.0f / distanceFromPlayer) / 2.0f;
                    //画在视野范围之内但是不要太近的物体, 不画超过视距的物体
                    //draw object witch is in FOV
                    if (inPlayerFOV && distanceFromPlayer >= 0.5f && distanceFromPlayer < depth)
                    {
                        // Work out its position on the floor...
                        olc::vf2d floorPoint;

                        // Horizontal screen location is determined based on object angle relative to camera heading
                        floorPoint.x = (objectAngle / FOV + 0.5f) * ScreenWidth();

                        // Vertical screen location is projected distance
                        //floorPoint.y = (ScreenHeight() / 2.0f) + (ScreenHeight() / distanceFromPlayer) / std::cos(objectAngle / 2.0f);
                        floorPoint.y = ScreenHeight() / 2.0f + ScreenHeight() / distanceFromPlayer;

                        // First we need the objects size...
                        olc::vf2d objectSize = { 1, 1 };
                        //olc::vf2d objectSize = { 0.1f, 1.0f };

                        // ...which we can scale into world space (maintaining aspect ratio)...
                        objectSize *= 2.0f * ScreenHeight();

                        // ...then project into screen space
                        objectSize /= distanceFromPlayer;

                        // Second we need the objects top left position in screen space...
                        // ...which is relative to the objects size and assumes the middle of the object is
                        // the location in world space
                        olc::vf2d objectTopLeft = { floorPoint.x - objectSize.x / 2.0f, floorPoint.y - objectSize.y };

                        // Now iterate through the objects screen pixels
                        for (float y = 0; y < objectSize.y; y++)
                        {
                            for (float x = 0; x < objectSize.x; x++)
                            {
                                // Create a normalised sample coordinate
                                float sampleX = x / objectSize.x;
                                float sampleY = y / objectSize.y;

                                // Calculate screen pixel location
                                olc::vi2d a = { int(objectTopLeft.x + x), int(objectTopLeft.y + y) };

                                wchar_t c = renderer->sprite->SampleGlyph(sampleX, sampleY);

                                // Check if location is actually on screen (to not go OOB on depth buffer)
                                if (a.x >= 0 && a.x < ScreenWidth() && a.y >= 0 && a.y < ScreenHeight() && c != L' ')
                                {
                                    short att = renderer->sprite->SampleColour(sampleX, sampleY);

                                    ConsoleColor foreColor = (ConsoleColor)(att & 0x000F);
                                    ConsoleColor backColor = (ConsoleColor)((att & 0x00F0) / 16);

                                    Color24 pixelColor = palette[foreColor];
                                    UNUSED(backColor);

                                    // Get pixel from a suitable texture
                                    float object_fHeading = 0.0f; //todo
                                    float niceAngle = playerAngle - object_fHeading + 3.14159f / 4.0f;
                                    if (niceAngle < 0) niceAngle += 2.0f * 3.14159f;
                                    if (niceAngle > 2.0f * 3.14159f) niceAngle -= 2.0f * 3.14159f;

                                    Pixel pixel = shade_object(sampleX, sampleY, pixelColor, distanceFromPlayer);
                                    // Draw the pixel taking into account the depth buffer
                                    DepthDraw(a.x, a.y, distanceFromPlayer, pixel);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    void render_hud(float deltaTime)
    {
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

        //draw player dot on the map
        Draw((int)playerX, (int)playerY, Pixel(255, 255, 255));

        if (enableHud)
        {
            weapon_Ypos = weapon_Xcof * weapon_Xcof;

            int drawPosX = 0;
            int drawPosY = 0;

            //draw weapon & draw muzzle flame
            if (weapon_current == WeaponEnum::DESERT_EAGLE)
            {
                drawPosX = 140 + int(weapon_Xcof * 4);
                drawPosY = int(60 - weapon_Ypos / 2);

                if (enableFlame)
                {
                    DisplaySprite(spriteExplosion, drawPosX + 13, drawPosY + 38, 2);
                }

                DisplaySprite(spriteDesertEagle, drawPosX, drawPosY, 1);
            }
            if (weapon_current == WeaponEnum::AK47)
            {
                //drawPosX = 100 + int(weapon_Xcof * 4);
                //drawPosY = int(-60 - weapon_Ypos);
                //if (enableFlame)
                //{
                //    DisplaySprite(spriteExplosion, drawPosX + 35, drawPosY + 150, 2);
                //}

                drawPosX = 50 + int(weapon_Xcof * 4);
                drawPosY = int(-120 - weapon_Ypos);

                if (enableFlame)
                {
                    DisplaySprite(spriteExplosion, drawPosX + 95, drawPosY + 205, 2);
                }

                DisplaySprite(spriteAK47, drawPosX, drawPosY, 1);
            }
            if (weapon_current == WeaponEnum::AEK_971) //rifle aeksu 971
            {
                drawPosX = 200 + int(weapon_Xcof * 4);
                drawPosY = int(0 - weapon_Ypos / 2);

                DisplaySprite(sptireWeapon_aek, drawPosX, drawPosY, 3);
            }
            if (weapon_current == WeaponEnum::M4A1)
            {
                drawPosX = 100 + int(weapon_Xcof * 4);
                drawPosY = int(-60 - weapon_Ypos);

                if (enableFlame)
                {
                    DisplaySprite(spriteExplosion, drawPosX + 60, drawPosY + 148, 2);
                }

                DisplaySprite(spriteM4A1, drawPosX, drawPosY, 2);
            }
            //TODO:rocket launcher (pls make sprite)

            //draw health bar
            if (enableBloodBar)
            {
                for (int y = 160, effect = 0; y <= 170; y++, effect++)
                {
                    for (int x = 200 - effect; x <= 200 - effect + playerHealth; x++)
                    {
                        Draw(x, y, Pixel(55 + (playerHealth * 2), 0, 0));
                    }
                }
            }
        }

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

    //you can add shader code here:
    Pixel shade_object(float sampleX, float sampleY, Color24 pixelColor, float distance)
    {
        Pixel pixel(pixelColor.r, pixelColor.g, pixelColor.b);

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

private:
    int year;
    int month;
    int day;
    int dayInWeek;

    //todo:
    bool todayIsChristmas = false;
    bool boxheadInConsole = false;

public:
    //this function is for Easter eggs:
    void SetDate(int year, int month, int day, int dayInWeek)
    {
        this->year = year;
        this->month = month;
        this->day = day;
        //set right value :)
        if (dayInWeek == 0) dayInWeek = 7;
        //[1, 7] => [Monday, Sunday]
        this->dayInWeek = dayInWeek;
    }

    void ReadGameSetting()
    {
        Database* database = nullptr;

        wstring relativePath = L"../../game_setting.txt";
        //find relative path first, then in exe folder.
        //read config file, if its not exist, create one.
        if (File::Exists(relativePath))
        {
            database = new Database(L"game_setting.txt", L"../../");
        }
        else
        {
            database = new Database(L"game_setting.txt");
        }

        this->enableMouse = database->GetBool(L"useMouse", false);
        this->__enableMouse = this->enableMouse;

        this->todayIsChristmas = database->GetBool(L"todayIsChristmas", false);
        this->boxheadInConsole = database->GetBool(L"boxheadInConsole", false);

        delete database;
    }

    PixelFPSDemo2() : GM(GameManager::Global.GetInstance())
    {
        this->sAppName = "PixelFPS Demo2";
    }

    // Called once on application startup, use to load your resources
    bool OnUserCreate() override
    {
        window = Window(this->__gameWindow);

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

        //load sprites:
        this->spriteWall = Resources::Load<OLCSprite>(L"../../", L"res/", L"fps_wall1.spr");
        this->spriteLamp = Resources::Load<OLCSprite>(L"../../", L"res/", L"fps_lamp1.spr");
        this->spriteFireBall = Resources::Load<OLCSprite>(L"../../", L"res/", L"fps_fireball1.spr");
        this->spriteExplosion = Resources::Load<OLCSprite>(L"../../", L"res/", L"fps_explosion.spr");
        this->spriteFlower = Resources::Load<OLCSprite>(L"../../", L"res/", L"flower.spr");
        this->sptireWeapon_aek = Resources::Load<OLCSprite>(L"../../", L"res/", L"aeksu_weapon.spr");
        this->spriteBullet = Resources::Load<OLCSprite>(L"../../", L"res/", L"fps_bullet.spr");
        this->spriteDesertEagle = Resources::Load<OLCSprite>(L"../../", L"res/", L"deagle.spr");
        this->spriteAK47 = Resources::Load<OLCSprite>(L"../../", L"res/", L"ak47.spr");
        //this->spriteM4A1 = Resources::Load<OLCSprite>(L"../../", L"res/", L"M4A1.spr");

        //load audios:
        this->bgm = Resources::Load<Audio>(L"../../", L"res/audios/", L"Silent Hill 2 OST - Laura Plays The Piano.mp3");
        this->bgm2 = Resources::Load<Audio>(L"../../", L"res/audios/", L"Silent Hill 2 OST - True.mp3");
        this->explosionSound = Resources::Load<Audio>(L"../../", L"res/audios/", L"548_Effect.Explosion.wav.mp3");
        this->fireBallSound = Resources::Load<Audio>(L"../../", L"res/audios/", L"560_Weapon.Rocket.Fire.wav.mp3");

        this->explosionPool = Resources::Load<AudioPool>(L"../../", L"res/audios/", L"548_Effect.Explosion.wav.mp3");
        this->fireBallPool = Resources::Load<AudioPool>(L"../../", L"res/audios/", L"560_Weapon.Rocket.Fire.wav.mp3");
        this->aekBulletPool = Resources::Load<AudioPool>(L"../../", L"res/audios/", L"aek_shot.mp3");
        this->dePool = Resources::Load<AudioPool>(L"../../", L"res/audios/weapons/", L"deagle-1.wav");
        this->ak47Pool = Resources::Load<AudioPool>(L"../../", L"res/audios/weapons/", L"ak47-1.wav");
        this->m4a1Pool = Resources::Load<AudioPool>(L"../../", L"res/audios/weapons/", L"m4a1_unsil-1.wav");

        //play bgm:
        this->bgm2->SetVolume(MCI_MAX_VOLUME / 3);
        this->bgm2->Play(false, false);

        GameObject* lamp1 = new GameObject();
        lamp1->transform->position = vf2d(8.5f, 8.5f);
        lamp1->AddComponent<SpriteRenderer>()->sprite = this->spriteLamp;

        GameObject* lamp2 = new GameObject();
        lamp2->transform->position = vf2d(7.5f, 7.5f);
        lamp2->AddComponent<SpriteRenderer>()->sprite = this->spriteLamp;

        GameObject* lamp3 = new GameObject();
        lamp3->transform->position = vf2d(10.5f, 3.5f);
        lamp3->AddComponent<SpriteRenderer>()->sprite = this->spriteLamp;

        //add weapons:
        Weapon* desertEagle = new Weapon(WeaponEnum::DESERT_EAGLE, WeaponType::Pistol, spriteDesertEagle);
        weapons.insert_or_assign((int)desertEagle->weapon_enum, desertEagle);

        Weapon* ak47 = new Weapon(WeaponEnum::AK47, WeaponType::Rifle, spriteAK47);
        weapons.insert_or_assign((int)ak47->weapon_enum, ak47);

        Weapon* aek_971 = new Weapon(WeaponEnum::AEK_971, WeaponType::Rifle, sptireWeapon_aek);
        weapons.insert_or_assign((int)aek_971->weapon_enum, aek_971);

        Weapon* m4a1 = new Weapon(WeaponEnum::M4A1, WeaponType::Rifle, spriteM4A1);
        weapons.insert_or_assign((int)m4a1->weapon_enum, m4a1);

        desertEagle->fire_interval = 0.45f;
        ak47->fire_interval = 0.1f;
        aek_971->fire_interval = 0.125f;
        m4a1->fire_interval = 0.08f;

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

        this->depthBuffer = new float[ScreenWidth() * ScreenHeight()];

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

        Debug::OutputLine(to_wstring(GM.gameObjects.size()));

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

        HWND foreWindow = GetForegroundWindow();
        if (foreWindow == this->__gameWindow)
        {
            receive_user_input(deltaTime);
        }

        render_world(deltaTime);

        render_hud(deltaTime);

        vector<int> readyToDeleteIds;
        for (auto& item : GM.gameObjects)
        {
            if (item.second->remove)
            {
                readyToDeleteIds.push_back(item.second->id);
            }
        }
        for (int& id : readyToDeleteIds)
        {
            GameObject* go_ref = GM.gameObjects[id];
            delete go_ref;
        }

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
        delete this->sptireWeapon_aek;

        delete this->bgm;
        delete this->bgm2;
        delete this->explosionSound;
        delete this->fireBallSound;
        delete this->explosionPool;
        delete this->fireBallPool;
        delete this->aekBulletPool;
        delete this->dePool;
        delete this->ak47Pool;
        delete this->m4a1Pool;

        delete[] this->depthBuffer;

        return true;
    }
};