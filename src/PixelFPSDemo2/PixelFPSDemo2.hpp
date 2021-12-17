#pragma once

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
#include "tool.hpp"
#include "PointLight.hpp" //point light
#include "EasyBMP.h" //BMP support
#include <unordered_map>
#include <thread> //thread support
#include "NetworkCollider.hpp" //net collider
#include "BMPRenderer.hpp" //bmp renderer
#include "PNGRenderer.hpp" //png renderer
#include "NetworkMessage.hpp" //net
#include "FPS_Server.hpp" //net server

inline void serverTask(FPSServer* server)
{
    while (true)
    {
        server->Update(-1, true);
    }
}

enum class GraphicalQuality
{
    Low = 1,
    Middle = 2,
    Hign = 3,
    Ultra = 4,
};

enum class GameMode
{
    Unknown = 0,
    SinglePlayer_SurvivalMode,
    MultiPlayer_SoloMode,
    MultiPlayer_ZombieEscapeMode,
};

class PixelFPSDemo2 : public PixelGameEngine, olc::net::client_interface<NetworkMessage>
{
private:
    //zombie mode:
    float zombieMaxHealth = 1000.0f;
    float zombieMoveSpeed = 4.0f;
    ZS_PlayerType zsPlayerType = ZS_PlayerType::Human;

    uint32_t InfectZombieRandom()
    {
        //dict => list:
        vector<pair<uint32_t, PlayerNetData>> playerList;

        //add all humans:
        for (const auto& item : this->mapObjects)
        {
            if (item.second.zs_PlayerType == ZS_PlayerType::Human)
            {
                playerList.push_back(item);
            }
        }

        //有幸存者:
        if (playerList.size() > 0)
        {
            int randomIndex = Random::Range(0, playerList.size() - 1);
            return playerList[randomIndex].first;
        }

        return 0;
    }

private:
    bool randomRespawnPos = true;

private:
    //object size:
    vf2d bulletSize = vf2d(0.1f, 0.1f);
    vf2d bulletPos = vf2d(1.0f, 4.0f);
    vf2d explosionSize = vf2d(0.2f, 0.2f);
    vf2d explosionPos = vf2d(0.0f, 1.6f);

private:
    //game mode:
    GameMode gameMode = GameMode::SinglePlayer_SurvivalMode;
    WeaponEnum soloWeapon = WeaponEnum::DESERT_EAGLE;

private:
    //Graphical Quality:
    GraphicalQuality graphicalQuality = GraphicalQuality::Middle;
    Color24 groundColor = Color24(212, 212, 212);
    Color24 skyColor = Color24(0, 0, 255);

private:
    //more hud:
    bool hitOther = false;
    float hitDamage = 0.0f;
    float hitOtherTimer = 0.0f;
    float hitOtherLastTime = 0.5f;

    //kill hud:
    bool killOther = false;
    float killOtherTimer = 0.0f;
    float killOtherLastTime = 0.75f;

private:
    //net setting:
    NetworkType networkType = NetworkType::None;
    string netIPAddress;
    const uint16_t net_port = 17971;

    //========server========
    FPSServer* server = nullptr;
    std::thread* serverThread = nullptr;

    //========client========
    bool waitingForConnection = true;
    uint32_t playerID = 0; //unknown at beginning.
    uint32_t playerID_BulletIndex = 1;
    std::unordered_map<uint32_t, PlayerNetData> mapObjects; //all player datas
    std::unordered_map<uint32_t, vf2d> networkObjectPositions; //cache, lerp
    std::unordered_map<uint32_t, GameObject*> networkObjects;
    std::unordered_map<uint32_t, vector<GameObject*>> networkBullets;
    vector<GameObject*> myBullets;

    //net ui:
    const float beHitEffectLastTime = 0.35f;
    bool netBeHit = false;
    float beHitEffectTimer = 0.0f;

private:
    //graphics setting:
    bool useOldRaycastObject = false;

private:
    //shader setting:
    bool enableFog = true;
    //Color24 defaultFogColor = Color24(192, 192, 192);
    Color24 defaultFogColor = Color24(0, 0, 0);
    bool renderBasedOnDistance = true;
    bool night = true;
    bool lightGround = true; //turn off by defualt, who can optimize it?

private:
    //BMP:
    //BMP* awp_bmp = nullptr;
    //BMP* m4a1_bmp = nullptr;
    //BMP* scope_bmp = nullptr;
    //BMP* GSG9_bmp = nullptr;
    //BMP* Zombie_bmp = nullptr;

private:
    olc::Sprite* de_png = nullptr;
    olc::Sprite* ak_png = nullptr;
    olc::Sprite* aek_png = nullptr;
    olc::Sprite* awp_png = nullptr;
    olc::Sprite* m4a1_png = nullptr;
    olc::Sprite* scope_png = nullptr;
    olc::Sprite* GSG9_png = nullptr;
    olc::Sprite* Zombie_png = nullptr;
    olc::Sprite* frontSight = nullptr;
    olc::Sprite* zombie_hand_png = nullptr;
    olc::Sprite* knife_png = nullptr;

private:
    //map:
    wstring map;
    int mapWidth = 32;
    int mapHeight = 32;

    //player:
    float playerX = 8.5f;           // Player Start Position
    float playerY = 14.7f;
    float playerAngle = 0.0f;       // Player Start Rotation(in rad)
    float moveSpeed = 2.5f;         // Walking Speed
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
    bool enableWeaponHud = true;
    bool enableBloodBar = true;
    int bloodBarPosX = 20;
    int bloodBarPosY = 150;

    //bobbing effect:
    float weapon_Ypos = 1.0f;
    float weapon_Xcof = 1.0f;
    bool bobbing_side = false;

    float bobbing_timer = 0.0f;
    const float bobbing_interval = 0.015f;

    //muzzle flame:
    bool enableFlame = false;
    const float flameInterval = 0.1f;
    float flameTimer = 0.0f;

    //camera:
    const float FOV = 3.14159f / 4; // Field of view
    const float depth = 64.0f;      // Maximum rendering distance

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

    bool startedPlayBGM = false;
    bool disableBGM = false;
    bool muteAll = false;

    //audios:
    Audio* bgm = nullptr;
    Audio* bgm2 = nullptr;
    AudioPool* explosionPool = nullptr;
    AudioPool* fireBallPool = nullptr;
    AudioPool* aekBulletPool = nullptr;
    AudioPool* dePool = nullptr;
    AudioPool* ak47Pool = nullptr;
    AudioPool* m4a1Pool = nullptr;
    AudioPool* awpPool = nullptr;

    //obstacles:
    std::vector<Vector2> obstacles;
    //for AI:
    //NOTE, sObject and listObjects can't interact normally with Navigation system.
    //So, im writing a new system.
    sObject* simple_ai;
    float nav_timer = 0;
    bool enableNav = false;

private:
    int year;
    int month;
    int day;
    int dayInWeek;

    //todo:
    bool todayIsChristmas = false;
    bool boxheadInConsole = false;

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

    void DrawBMP(BMP* bmp, int posX, int posY, int zoomPixelScaler = 1, float _m = 0.0f)
    {
        for (size_t y = 0; y < bmp->TellHeight(); y++)
        {
            for (size_t x = 0; x < bmp->TellWidth(); x++)
            {
                RGBApixel color = bmp->GetPixel(x, y);

                //treat pure white pixel as alpha transparency.
                if (color.Red == 255 && color.Green == 255 && color.Blue == 255)
                {
                    continue;
                }

                //light:
                Pixel pixel(color.Red, color.Green, color.Blue);

                pixel.r = fuck_std::clamp<float>(pixel.r * (1 + _m), 0, 255);
                pixel.g = fuck_std::clamp<float>(pixel.g * (1 + _m), 0, 255);
                pixel.b = fuck_std::clamp<float>(pixel.b * (1 + _m), 0, 255);

                Draw(posX + x, posY + y, pixel);
            }
        }
    }

    void DrawPNG(olc::Sprite* sprite, int posX, int posY, int zoomPixelScaler = 1, float _m = 0.0f)
    {
        Sprite* copySprite = sprite->Duplicate();

        for (size_t y = 0; y < sprite->height; y++)
        {
            for (size_t x = 0; x < sprite->width; x++)
            {
                Pixel pixel = sprite->GetPixel(x, y);

                pixel.r = fuck_std::clamp<float>(pixel.r * (1 + _m), 0, 255);
                pixel.g = fuck_std::clamp<float>(pixel.g * (1 + _m), 0, 255);
                pixel.b = fuck_std::clamp<float>(pixel.b * (1 + _m), 0, 255);

                copySprite->SetPixel(x, y, pixel);
            }
        }

        SetPixelMode(olc::Pixel::ALPHA);  // Draw all pixels
        DrawSprite(posX, posY, copySprite, zoomPixelScaler);
        SetPixelMode(olc::Pixel::NORMAL); // Draw all pixels

        delete copySprite;
        copySprite = nullptr;
    }

    void DrawAlphaPanel(Color32 color)
    {
        SetPixelMode(olc::Pixel::ALPHA);  // Enable Alpha transparency
        for (size_t i = 0; i < ScreenHeight(); i++)
        {
            for (size_t j = 0; j < ScreenWidth(); j++)
            {
                Draw(j, i, Pixel(color.r, color.g, color.b, color.a));
            }
        }
        SetPixelMode(olc::Pixel::NORMAL); // Draw all pixels
    }

private:
    void TurnToHuman()
    {

    }

    void TurnToZombie()
    {
        //clear all human weapons:
        weapons.clear();

        Weapon* claw = new Weapon(WeaponEnum::ZombieEvilClaw, WeaponType::Knife, nullptr);
        claw->fire_interval = 1.5f;
        claw->damage = 120.0f;
        weapons.insert_or_assign((int)claw->weapon_enum, claw);

        weapon_current = claw->weapon_enum;

        this->playerHealth = this->zombieMaxHealth;
        this->moveSpeed = this->zombieMoveSpeed;
        this->zsPlayerType = ZS_PlayerType::Zombie;
    }

    void ChangeGameMode(GameMode gameMode)
    {
        if (gameMode == GameMode::SinglePlayer_SurvivalMode)
        {
            //add weapons:
            Weapon* desertEagle = new Weapon(WeaponEnum::DESERT_EAGLE, WeaponType::Pistol, spriteDesertEagle);
            weapons.insert_or_assign((int)desertEagle->weapon_enum, desertEagle);

            Weapon* ak47 = new Weapon(WeaponEnum::AK47, WeaponType::Rifle, spriteAK47);
            weapons.insert_or_assign((int)ak47->weapon_enum, ak47);

            Weapon* aek_971 = new Weapon(WeaponEnum::AEK_971, WeaponType::Rifle, sptireWeapon_aek);
            weapons.insert_or_assign((int)aek_971->weapon_enum, aek_971);

            Weapon* m4a1 = new Weapon(WeaponEnum::M4A1, WeaponType::Rifle, spriteM4A1);
            weapons.insert_or_assign((int)m4a1->weapon_enum, m4a1);

            Weapon* awp = new Weapon(WeaponEnum::AWP, WeaponType::Sniper, nullptr);
            weapons.insert_or_assign((int)awp->weapon_enum, awp);

            desertEagle->fire_interval = 0.45f;
            desertEagle->damage = 25.5f;

            ak47->fire_interval = 0.1f;
            ak47->damage = 15.5f;

            aek_971->fire_interval = 0.125f;
            aek_971->damage = 18.6f;

            m4a1->fire_interval = 0.08f;
            m4a1->damage = 12.7f;

            awp->fire_interval = 1.5f;
            awp->damage = 65.4f;
        }
        else if (gameMode == GameMode::MultiPlayer_SoloMode)
        {
            Weapon* weapon = nullptr;
            switch (soloWeapon)
            {
            case WeaponEnum::DESERT_EAGLE:
                weapon = new Weapon(WeaponEnum::DESERT_EAGLE, WeaponType::Pistol, spriteDesertEagle);
                weapon->fire_interval = 0.35f;
                weapon->damage = 35.5f;
                break;
            case WeaponEnum::AK47:
                weapon = new Weapon(WeaponEnum::AK47, WeaponType::Rifle, spriteAK47);
                weapon->fire_interval = 0.15f;
                weapon->damage = 11.5f;
                break;
            case WeaponEnum::M4A1:
                weapon = new Weapon(WeaponEnum::M4A1, WeaponType::Rifle, nullptr);
                weapon->fire_interval = 0.12f;
                weapon->damage = 7.7f;
                break;
            case WeaponEnum::AWP:
                weapon = new Weapon(WeaponEnum::AWP, WeaponType::Sniper, nullptr);
                weapon->fire_interval = 1.9f;
                weapon->damage = 85.4f;
                break;
            case WeaponEnum::AEK_971:
                weapon = new Weapon(WeaponEnum::AEK_971, WeaponType::Rifle, sptireWeapon_aek);
                weapon->fire_interval = 0.125f;
                weapon->damage = 5.6f;
                break;
            default:
                throw "??? Unknown Weapon ???";
                break;
            }

            weapon_current = weapon->weapon_enum;
            weapons.insert_or_assign((int)weapon->weapon_enum, weapon);
        }
        else if (gameMode == GameMode::MultiPlayer_ZombieEscapeMode)
        {
            //add weapons:
            Weapon* desertEagle = new Weapon(WeaponEnum::DESERT_EAGLE, WeaponType::Pistol, spriteDesertEagle);
            weapons.insert_or_assign((int)desertEagle->weapon_enum, desertEagle);

            Weapon* ak47 = new Weapon(WeaponEnum::AK47, WeaponType::Rifle, spriteAK47);
            weapons.insert_or_assign((int)ak47->weapon_enum, ak47);

            Weapon* knife = new Weapon(WeaponEnum::KNIFE, WeaponType::Knife, nullptr);
            weapons.insert_or_assign((int)knife->weapon_enum, knife);

            desertEagle->fire_interval = 0.75f;
            desertEagle->damage = 225.5f;

            ak47->fire_interval = 0.1f;
            ak47->damage = 57.2f;

            knife->fire_interval = 1.5f;
            knife->damage = 575.0f;

            weapon_current = WeaponEnum::AK47;
        }
    }

    void Respawn()
    {
        if (gameMode == GameMode::MultiPlayer_ZombieEscapeMode)
        {
            if (zsPlayerType == ZS_PlayerType::Human)
            {
                this->playerHealth = this->fullHealth;
            }
            else if (zsPlayerType == ZS_PlayerType::Zombie)
            {
                this->playerHealth = this->zombieMaxHealth;
            }
        }
        else
        {
            this->playerHealth = this->fullHealth;
        }
        this->playerAngle = 0.0f;

        if (randomRespawnPos)
        {
            vi2d randomPos = get_random_available_pos();
            this->playerX = randomPos.x + 0.5f;
            this->playerY = randomPos.y + 0.5f;
        }
        else
        {
            this->playerX = 8.5f;
            this->playerY = 14.7f;
        }

        beHitEffectTimer = 0.0f;
        netBeHit = false;

        if (networkType != NetworkType::None)
        {
            IRespawn iRespawn;
            iRespawn.uniqueID = playerID;
            iRespawn.health = playerHealth;
            iRespawn.posX = playerX;
            iRespawn.posY = playerY;

            olc::net::message<NetworkMessage> msg;
            msg.header.id = NetworkMessage::Game_IRespawn;
            msg.AddBytes(iRespawn.Serialize());
            Send(msg);
        }
    }

    void Net_HitOther(uint32_t otherID)
    {
        if (networkType != NetworkType::None)
        {
            BulletHitInfo info;
            info.myID = playerID;
            info.otherPlayerID = otherID;
            info.damage = weapons[(int)weapon_current]->damage;
            olc::net::message<NetworkMessage> msg;
            msg.header.id = NetworkMessage::Game_BulletHitOther;
            msg.AddBytes(info.Serialize());
            Send(msg);
        }
    }

    void Net_Knife_Detect()
    {
        //近战武器攻击检测:
        for (const auto& ob : networkObjects)
        {
            if (ob.first == playerID) continue; //不要检测自己
            if (!ob.second->active) continue; //不要鞭尸

            GameObject* otherGo = ob.second;

            float vecX = otherGo->transform->position.x - playerX;
            float vecY = otherGo->transform->position.y - playerY;
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

            //hit:
            if (inPlayerFOV && distanceFromPlayer <= 2.5f)
            {
                //display:
                make_hit_hud(weapons[(int)weapon_current]->damage);

                Net_HitOther(ob.first);
            }
        }
    }

private:
    void update_audio()
    {
        if (!muteAll)
        {
            //check audio state:
            if (this->bgm2->IsOver() && !startedPlayBGM)
            {
                startedPlayBGM = true;

                this->bgm->SetVolume(MCI_MAX_VOLUME / 3);
                this->bgm->Play(true, false);
            }

            //clean audio pool:
            explosionPool->Clean();
            fireBallPool->Clean();
            aekBulletPool->Clean();
            dePool->Clean();
            ak47Pool->Clean();
            m4a1Pool->Clean();
            awpPool->Clean();
        }
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

    void weapon_bobbing(float deltaTime)
    {
        bobbing_timer += deltaTime;

        if (bobbing_timer >= bobbing_interval)
        {
            bobbing_timer = 0.0f;

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

                weapon_bobbing(deltaTime);
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

                weapon_bobbing(deltaTime);
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
        if (GetKey(Key::X).bHeld)
        {
            playerHealth -= selfDamage;

            this->netBeHit = true;
        }

        //infect people randomly if you are host:
        if (this->networkType == NetworkType::Host)
        {
            if (this->gameMode == GameMode::MultiPlayer_ZombieEscapeMode)
            {
                if (GetKey(Key::I).bPressed)
                {
                    uint32_t randomID = InfectZombieRandom();
                    //如果成功感染
                    if (randomID != 0)
                    {
                        olc::net::message<NetworkMessage> msg;
                        msg.header.id = NetworkMessage::Game_Zombie_Infect;

                        ZombieInfect zombieInfect;
                        zombieInfect.targetID = randomID;
                        zombieInfect.uniqueID = this->playerID;
                        msg.AddBytes(zombieInfect.Serialize());
                        Send(msg);

                        //如果自己尸变:
                        if (randomID == playerID)
                        {
                            TurnToZombie();
                        }
                    }
                }
            }
        }

        WeaponEnum beforeCurrentWeapon = weapon_current;

        //switch weapon:
        if (GetKey(Key::K1).bPressed)
        {
            weapon_current = WeaponEnum::AK47;
        }
        if (GetKey(Key::K2).bPressed)
        {
            weapon_current = WeaponEnum::DESERT_EAGLE;
        }
        if (GetKey(Key::K3).bPressed)
        {
            weapon_current = WeaponEnum::KNIFE;
        }
        if (GetKey(Key::K4).bPressed)
        {
            weapon_current = WeaponEnum::M4A1;
        }
        if (GetKey(Key::K5).bPressed)
        {
            weapon_current = WeaponEnum::AWP;
        }
        if (GetKey(Key::K6).bPressed)
        {
            weapon_current = WeaponEnum::AEK_971;
        }

        if (weapons.count((int)weapon_current) == 0)
        {
            weapon_current = beforeCurrentWeapon;
        }

        Weapon* weapon = weapons[(int)weapon_current];

        //sniper scope:
        if (weapon->weapon_type == WeaponType::Sniper)
        {
            weapon->openScope = GetMouse(Mouse::RIGHT).bHeld;
            if (weapon->openScope)
            {
                moveSpeed = 1.0f;
            }
            else
            {
                moveSpeed = 2.5f;
            }
        }

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
                if (weapon->weapon_enum == WeaponEnum::ZombieEvilClaw)
                {
                    //todo: play animation
                    Net_Knife_Detect();
                }
                else if (weapon->weapon_enum == WeaponEnum::KNIFE)
                {
                    //todo: play animation
                    Net_Knife_Detect();
                }
                else
                {
                    GameObject* bullet = new GameObject();

                    //set position:
                    bullet->transform->position = vf2d(playerX, playerY);

                    //add collider:
                    bullet->AddComponent<Collider>();

                    //add point light:
                    bullet->AddComponent<PointLight>(2.0f);

                    //add sprite:
                    SpriteRenderer* renderer = bullet->AddComponent<SpriteRenderer>();
                    renderer->sprite = this->spriteBullet;
                    renderer->ObjectSize = this->bulletSize;
                    renderer->ObjectPos = this->bulletPos;

                    //make noise:
                    float noise = (((float)rand() / (float)RAND_MAX) - 0.5f) * 0.1f;
                    //bullet speed:
                    float bulletSpeed = 30;

                    if (weapon->weapon_enum == WeaponEnum::DESERT_EAGLE)
                    {
                        if (!muteAll)
                        {
                            //play fire sound:
                            dePool->PlayOneShot(0.5f);
                        }
                        bulletSpeed = 40;
                    }
                    if (weapon->weapon_enum == WeaponEnum::AK47)
                    {
                        if (!muteAll)
                        {
                            //play fire sound:
                            ak47Pool->PlayOneShot(0.5f);
                        }
                    }
                    if (weapon->weapon_enum == WeaponEnum::AEK_971)
                    {
                        if (!muteAll)
                        {
                            //play fire sound:
                            aekBulletPool->PlayOneShot(0.5f);
                        }
                    }
                    if (weapon->weapon_enum == WeaponEnum::M4A1)
                    {
                        if (!muteAll)
                        {
                            //play fire sound:
                            m4a1Pool->PlayOneShot(0.5f);
                        }
                        bulletSpeed = 32.0f;
                        noise = (((float)rand() / (float)RAND_MAX) - 0.5f) * 0.07f;
                    }
                    if (weapon->weapon_enum == WeaponEnum::AWP)
                    {
                        if (!muteAll)
                        {
                            //play fire sound:
                            awpPool->PlayOneShot(0.5f);
                        }
                        bulletSpeed = 50.0f;
                        if (weapon->openScope)
                        {
                            noise = 0.0f;
                        }
                        else
                        {
                            noise = (((float)rand() / (float)RAND_MAX) - 0.5f) * 0.2f;
                        }
                    }

                    float vx = cosf(playerAngle + noise) * bulletSpeed;
                    float vy = sinf(playerAngle + noise) * bulletSpeed;

                    //set velocity:
                    bullet->transform->velocity = vf2d(vx, vy);

                    //set muzzle flame timer:
                    if (!enableFlame)
                    {
                        enableFlame = true;
                    }

                    //net:sync bullet:
                    if (this->networkType != NetworkType::None)
                    {
                        myBullets.push_back(bullet);
                        mapObjects[playerID].bullets.push_back(NetBullet(this->playerID_BulletIndex, bullet->transform->position.x, bullet->transform->position.y));

                        bullet->AddComponent<NetworkCollider>(this->playerID_BulletIndex);
                        this->playerID_BulletIndex++;
                    }
                }
            }
        }
    }

    void update_network(float deltaTime)
    {
        if (this->networkType == NetworkType::None)
        {
            return;
        }

        //fuck_std::debug_output_line(to_wstring(mapObjects[playerID].bullets.size()));

        //remove all bullets:
        for (const auto& item : networkBullets)
        {
            for (const auto& go : item.second)
            {
                delete go;
            }
        }
        networkBullets.clear();

        for (const auto& object : mapObjects)
        {
            if (object.first == playerID)
            {
                continue; //网络行为不负责模拟自己
            }

            if (object.second.zs_PlayerType == ZS_PlayerType::Human)
            {
                auto renderer = networkObjects[object.first]->GetComponent<PNGRenderer>();
                renderer->sprite = GSG9_png;
            }
            else if (object.second.zs_PlayerType == ZS_PlayerType::Zombie)
            {
                auto renderer = networkObjects[object.first]->GetComponent<PNGRenderer>();
                renderer->sprite = Zombie_png;
            }

            //add new bullets:
            for (const NetBullet& bullet : object.second.bullets)
            {
                GameObject* newBullet = new GameObject();

                //set position:
                newBullet->transform->position = vf2d(bullet.x, bullet.y);

                //add point light:
                newBullet->AddComponent<PointLight>(2.0f);

                //add sprite:
                auto renderer = newBullet->AddComponent<SpriteRenderer>();
                renderer->sprite = this->spriteBullet;
                renderer->ObjectSize = this->bulletSize;
                renderer->ObjectPos = this->bulletPos;

                networkBullets[object.first].push_back(newBullet);
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

    void make_kill_hud()
    {
        this->killOther = true;
        this->killOtherTimer = 0.0f;
    }

    void draw_kill_hud(float deltaTime)
    {
        if (killOther)
        {
            killOtherTimer += deltaTime;

            if (killOtherTimer >= killOtherLastTime)
            {
                this->killOther = false;
                this->killOtherTimer = 0.0f;
            }

            //display:
            DrawString(125, 75, "KILL!!!", Pixel(255, 140, 0));
        }
    }

    void make_hit_hud(float damage)
    {
        this->hitOther = true;
        this->hitOtherTimer = 0.0f;
        this->hitDamage = damage;
    }

    void draw_hit_hud(float deltaTime)
    {
        if (hitOther)
        {
            hitOtherTimer += deltaTime;

            if (hitOtherTimer >= hitOtherLastTime)
            {
                hitOther = false;
                hitOtherTimer = 0.0f;
            }

            DrawString(50, 50, to_string(hitDamage));
        }
    }

    void make_behit_hud()
    {
        //todo
    }

    void draw_behit_hud(float deltaTime)
    {
        if (this->netBeHit)
        {
            beHitEffectTimer += deltaTime;
            //stop it
            if (beHitEffectTimer >= beHitEffectLastTime)
            {
                beHitEffectTimer = 0.0f;
                this->netBeHit = false;
            }
            //display:
            DrawAlphaPanel(Color32(255, 0, 0, 100));
        }
    }

    void update_world(float deltaTime)
    {
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

                        if (!muteAll)
                        {
                            //play explosion sound:
                            explosionPool->PlayOneShot();
                        }

                        //instantiate explosion:
                        //这里创建新的游戏物体会不会导致遍历出现问题？需要进行测试
                        GameObject* explosion = new GameObject();
                        explosion->transform->position =
                            go->transform->position - go->transform->velocity * deltaTime;

                        PointLight* pl = explosion->AddComponent<PointLight>(3.0f);
                        pl->attenuation = 3.0f;

                        SpriteRenderer* renderer = explosion->AddComponent<SpriteRenderer>();
                        renderer->sprite = this->spriteExplosion;
                        renderer->ObjectSize = this->explosionSize;
                        renderer->ObjectPos = this->explosionPos;

                        LifeController* life = explosion->AddComponent<LifeController>();
                        life->lifeTime = 0.25f;

                        if (networkType != NetworkType::None)
                        {
                            NetworkCollider* net_collider = go->GetComponent<NetworkCollider>();
                            if (net_collider != nullptr && net_collider->enable)
                            {
                                int removeIndex = -1;
                                for (size_t _i = 0; _i < mapObjects[playerID].bullets.size(); _i++)
                                {
                                    if (mapObjects[playerID].bullets[_i].id == net_collider->networkID)
                                    {
                                        removeIndex = _i;
                                    }
                                }
                                if (removeIndex != -1)
                                {
                                    myBullets.erase(myBullets.begin() + removeIndex);
                                    mapObjects[playerID].bullets.erase(mapObjects[playerID].bullets.begin() + removeIndex);
                                }
                            }
                        }
                    }

                    //collsion with other objects:
                    if (this->networkType != NetworkType::None)
                    {
                        for (const auto& ob : networkObjects)
                        {
                            if (ob.first == playerID) continue; //不要检测自己
                            if (!ob.second->active) continue; //不要鞭尸

                            int bx = (int)go->transform->position.x;
                            int by = (int)go->transform->position.y;

                            //if hit:
                            if ((int)ob.second->transform->position.x == bx && (int)ob.second->transform->position.y == by)
                            {
                                go->remove = true;

                                NetworkCollider* net_collider = go->GetComponent<NetworkCollider>();
                                if (net_collider != nullptr && net_collider->enable)
                                {
                                    int removeIndex = -1;
                                    for (size_t _i = 0; _i < mapObjects[playerID].bullets.size(); _i++)
                                    {
                                        if (mapObjects[playerID].bullets[_i].id == net_collider->networkID)
                                        {
                                            removeIndex = _i;
                                        }
                                    }
                                    if (removeIndex != -1)
                                    {
                                        myBullets.erase(myBullets.begin() + removeIndex);
                                        mapObjects[playerID].bullets.erase(mapObjects[playerID].bullets.begin() + removeIndex);
                                    }
                                }

                                //display:
                                make_hit_hud(weapons[(int)weapon_current]->damage);

                                BulletHitInfo info;
                                info.myID = playerID;
                                info.otherPlayerID = ob.first;
                                info.damage = weapons[(int)weapon_current]->damage;
                                olc::net::message<NetworkMessage> msg;
                                msg.header.id = NetworkMessage::Game_BulletHitOther;
                                msg.AddBytes(info.Serialize());
                                Send(msg);
                            }
                        }
                    }
                }
                else
                {
                    go->remove = true;

                    if (networkType != NetworkType::None)
                    {
                        NetworkCollider* net_collider = go->GetComponent<NetworkCollider>();
                        if (net_collider != nullptr && net_collider->enable)
                        {
                            int removeIndex = -1;
                            for (size_t _i = 0; _i < mapObjects[playerID].bullets.size(); _i++)
                            {
                                if (mapObjects[playerID].bullets[_i].id == net_collider->networkID)
                                {
                                    removeIndex = _i;
                                }
                            }
                            if (removeIndex != -1)
                            {
                                myBullets.erase(myBullets.begin() + removeIndex);
                                mapObjects[playerID].bullets.erase(mapObjects[playerID].bullets.begin() + removeIndex);
                            }
                        }
                    }
                }
            }

            //灯光衰减:
            PointLight* pointLightCom = go->GetComponent<PointLight>();
            if (pointLightCom != nullptr && pointLightCom->enable)
            {
                pointLightCom->range -= pointLightCom->attenuation * deltaTime;
                if (pointLightCom->range < 0.0f)
                {
                    pointLightCom->range = 0.0f;
                }
            }

        }
    }

    void render_world(float deltaTime)
    {
        //reset buffer array:
        for (size_t i = 0; i < ScreenWidth() * ScreenHeight(); i++)
        {
            this->depthBuffer[i] = INFINITY;
        }

        //calculate point light distance:
        vector<PointLight*> pointLights;
        for (auto& item : GM.gameObjects)
        {
            GameObject* go = item.second;
            if (!go->active) continue;
            if (go->remove) continue;
            PointLight* pointLight = go->GetComponent<PointLight>();
            if (pointLight != nullptr && pointLight->enable)
            {
                pointLights.push_back(pointLight);
            }
        }

        //raycast:
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

            int ceiling = (int)(ScreenHeight() / 2.0f - ScreenHeight() / (distanceToWall * cosf(diffAngle)));
            int floor = ScreenHeight() - ceiling;

            float _m = 0.0f;
            for (const PointLight* pointLight : pointLights)
            {
                float distanceToPointLight = (pointLight->gameObject->transform->position - hitInfo.hitMapPos).mag();
                _m += max(0.0f, 1.0f - min(distanceToPointLight / pointLight->range, 1.0f));
            }

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

                    Pixel pixel = shade(planeTileX, planeTileY, CellSide::Top, skyColor, planeSampleX, planeSampleY, planeZ, 0, pointLights);
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

                        Pixel pixel = shade(hitInfo.hitMapPos.x, hitInfo.hitMapPos.y, hitInfo.side, pixelColor, sampleX, sampleY, distanceToWall, _m, pointLights);
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

                    Pixel pixel = shade(planeTileX, planeTileY, CellSide::Bottom, groundColor, planeSampleX, planeSampleY, planeZ, 0, pointLights);
                    Draw(x, y, pixel);
                }
            }
        }

        //render game objects:
        for (auto& item : GM.gameObjects)
        {
            GameObject* go = item.second;

            if (!go->active) continue;
            if (go->remove) continue;

            //sprite renderer:
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

                        //point light:
                        float _m = 0.0f;
                        for (const auto& light : pointLights)
                        {
                            float distanceToPointLight = (light->gameObject->transform->position - go->transform->position).mag();
                            _m += max(0.0f, 1.0f - min(distanceToPointLight / light->range, 1.0f));
                        }
                        _m = fuck_std::clamp<float>(_m, 0.0f, 0.3f);

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

                                        //draw objects:
                                        Pixel pixel = shade_object((int)go->transform->position.x, (int)go->transform->position.y, sampleX, sampleY, pixelColor, distanceFromPlayer, _m);
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
                        //point light:
                        float _m = 0.0f;
                        for (const auto& light : pointLights)
                        {
                            float distanceToPointLight = (light->gameObject->transform->position - go->transform->position).mag();
                            _m += max(0.0f, 1.0f - min(distanceToPointLight / light->range, 1.0f));
                        }
                        _m = fuck_std::clamp<float>(_m, 0.0f, 0.3f);

                        // Work out its position on the floor...
                        olc::vf2d floorPoint;

                        // Horizontal screen location is determined based on object angle relative to camera heading
                        floorPoint.x = (objectAngle / FOV + 0.5f) * ScreenWidth();

                        // Vertical screen location is projected distance
                        //floorPoint.y = (ScreenHeight() / 2.0f) + (ScreenHeight() / distanceFromPlayer) / std::cos(objectAngle / 2.0f);
                        floorPoint.y = ScreenHeight() / 2.0f + ScreenHeight() / distanceFromPlayer;

                        // First we need the objects size...
                        olc::vf2d objectSize = renderer->ObjectSize;

                        // ...which we can scale into world space (maintaining aspect ratio)...
                        objectSize *= 2.0f * ScreenHeight();

                        // ...then project into screen space
                        objectSize /= distanceFromPlayer;

                        // Second we need the objects top left position in screen space...
                        // ...which is relative to the objects size and assumes the middle of the object is
                        // the location in world space
                        //olc::vf2d objectTopLeft = { floorPoint.x - objectSize.x / 2.0f, floorPoint.y - objectSize.y };
                        olc::vf2d objectTopLeft = { floorPoint.x - objectSize.x / 2.0f, floorPoint.y - objectSize.y - renderer->ObjectPos.y * objectSize.y };

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

                                    Pixel pixel = shade_object((int)go->transform->position.x, (int)go->transform->position.y, sampleX, sampleY, pixelColor, distanceFromPlayer, _m);
                                    // Draw the pixel taking into account the depth buffer
                                    DepthDraw(a.x, a.y, distanceFromPlayer, pixel);
                                }
                            }
                        }
                    }
                }
            }

            //png renderer:
            PNGRenderer* png_renderer = go->GetComponent<PNGRenderer>();
            if (png_renderer != nullptr && png_renderer->enable)
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
                    olc::vf2d objectSize = png_renderer->ObjectSize;

                    // ...which we can scale into world space (maintaining aspect ratio)...
                    objectSize *= 2.0f * ScreenHeight();

                    // ...then project into screen space
                    objectSize /= distanceFromPlayer;

                    // Second we need the objects top left position in screen space...
                    // ...which is relative to the objects size and assumes the middle of the object is
                    // the location in world space
                    olc::vf2d objectTopLeft = { floorPoint.x - objectSize.x / 2.0f, floorPoint.y - objectSize.y };

                    //point light:
                    float _m = 0.0f;
                    for (const auto& light : pointLights)
                    {
                        float distanceToPointLight = (light->gameObject->transform->position - go->transform->position).mag();
                        _m += max(0.0f, 1.0f - min(distanceToPointLight / light->range, 1.0f));
                    }
                    _m = fuck_std::clamp<float>(_m, 0.0f, 0.3f);

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

                            // Check if location is actually on screen (to not go OOB on depth buffer)
                            if (a.x >= 0 && a.x < ScreenWidth() && a.y >= 0 && a.y < ScreenHeight())
                            {
                                Pixel pixel = png_renderer->SampleColour(sampleX, sampleY);

                                //treat pure white pixel as alpha transparency.
                                if (pixel.r == 255 && pixel.g == 255 && pixel.b == 255)
                                {
                                    continue;
                                }
                                //treat pure black pixel as alpha transparency.
                                if (pixel.r == 0 && pixel.g == 0 && pixel.b == 0)
                                {
                                    continue;
                                }

                                // Get pixel from a suitable texture
                                float object_fHeading = 0.0f; //todo
                                float niceAngle = playerAngle - object_fHeading + 3.14159f / 4.0f;
                                if (niceAngle < 0) niceAngle += 2.0f * 3.14159f;
                                if (niceAngle > 2.0f * 3.14159f) niceAngle -= 2.0f * 3.14159f;

                                pixel = shade_object((int)go->transform->position.x, (int)go->transform->position.y, sampleX, sampleY, Color24(pixel.r, pixel.g, pixel.b), distanceFromPlayer, _m);
                                // Draw the pixel taking into account the depth buffer
                                DepthDraw(a.x, a.y, distanceFromPlayer, pixel);
                            }
                        }
                    }
                }
            }

            continue;

            //bmp renderer:
            BMPRenderer* bmp_renderer = go->GetComponent<BMPRenderer>();
            if (bmp_renderer != nullptr && bmp_renderer->enable)
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
                    olc::vf2d objectSize = bmp_renderer->ObjectSize;

                    // ...which we can scale into world space (maintaining aspect ratio)...
                    objectSize *= 2.0f * ScreenHeight();

                    // ...then project into screen space
                    objectSize /= distanceFromPlayer;

                    // Second we need the objects top left position in screen space...
                    // ...which is relative to the objects size and assumes the middle of the object is
                    // the location in world space
                    olc::vf2d objectTopLeft = { floorPoint.x - objectSize.x / 2.0f, floorPoint.y - objectSize.y };

                    //point light:
                    float _m = 0.0f;
                    for (const auto& light : pointLights)
                    {
                        float distanceToPointLight = (light->gameObject->transform->position - go->transform->position).mag();
                        _m += max(0.0f, 1.0f - min(distanceToPointLight / light->range, 1.0f));
                    }
                    _m = fuck_std::clamp<float>(_m, 0.0f, 0.3f);

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

                            // Check if location is actually on screen (to not go OOB on depth buffer)
                            if (a.x >= 0 && a.x < ScreenWidth() && a.y >= 0 && a.y < ScreenHeight())
                            {
                                Color32 color32 = bmp_renderer->SampleColour(sampleX, sampleY);

                                //treat pure white pixel as alpha transparency.
                                if (color32.r == 255 && color32.g == 255 && color32.b == 255)
                                {
                                    continue;
                                }

                                Color24 pixelColor = Color24(color32.r, color32.g, color32.b);

                                // Get pixel from a suitable texture
                                float object_fHeading = 0.0f; //todo
                                float niceAngle = playerAngle - object_fHeading + 3.14159f / 4.0f;
                                if (niceAngle < 0) niceAngle += 2.0f * 3.14159f;
                                if (niceAngle > 2.0f * 3.14159f) niceAngle -= 2.0f * 3.14159f;

                                Pixel pixel = shade_object((int)go->transform->position.x, (int)go->transform->position.y, sampleX, sampleY, pixelColor, distanceFromPlayer, _m);
                                // Draw the pixel taking into account the depth buffer
                                DepthDraw(a.x, a.y, distanceFromPlayer, pixel);
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

        //draw health bar
        if (enableBloodBar && false)
        {
            for (int y = 160, effect = 0; y <= 170; y++, effect++)
            {
                for (int x = 200 - effect; x <= 200 - effect + playerHealth; x++)
                {
                    Draw(x, y, Pixel(55 + (playerHealth * 2), 0, 0));
                }
            }
        }

        //draw .spr weapon hud:(disabled)
        if (enableWeaponHud && false)
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
        }

        //draw .bmp weapon hud:(enabled)
        if (enableWeaponHud)
        {
            weapon_Ypos = weapon_Xcof * weapon_Xcof;

            int drawPosX = 0;
            int drawPosY = 0;

            //calculate point light distance:
            vector<PointLight*> pointLights;
            for (auto& item : GM.gameObjects)
            {
                GameObject* go = item.second;
                if (!go->active) continue;
                if (go->remove) continue;
                PointLight* pointLight = go->GetComponent<PointLight>();
                if (pointLight != nullptr && pointLight->enable)
                {
                    pointLights.push_back(pointLight);
                }
            }

            float _m = 0.0f;
            for (PointLight* pointLight : pointLights)
            {
                float distanceToPointLight = (pointLight->gameObject->transform->position - vf2d(playerX, playerY)).mag();
                _m += max(0.0f, 1.0f - min(distanceToPointLight / pointLight->range, 1.0f));
            }

            //draw weapon & draw muzzle flame
            if (weapon_current == WeaponEnum::DESERT_EAGLE)
            {
                drawPosX = int(weapon_Xcof * 2);
                drawPosY = int(weapon_Ypos / 2);

                if (enableFlame)
                {
                    //todo
                }

                DrawPNG(this->de_png, drawPosX, drawPosY, 1, _m);
            }
            else if (weapon_current == WeaponEnum::AK47)
            {
                drawPosX = int(weapon_Xcof * 2);
                drawPosY = int(weapon_Ypos / 2);

                if (enableFlame)
                {
                    //todo
                }

                DrawPNG(this->ak_png, drawPosX, drawPosY, 1, _m);
            }
            else if (weapon_current == WeaponEnum::AEK_971) //rifle aeksu 971
            {
                drawPosX = int(weapon_Xcof * 2);
                drawPosY = int(weapon_Ypos / 2);

                if (enableFlame)
                {
                    //todo
                }

                DrawPNG(this->aek_png, drawPosX, drawPosY, 1, _m);
            }
            else if (weapon_current == WeaponEnum::M4A1)
            {
                drawPosX = int(weapon_Xcof * 2);
                drawPosY = int(weapon_Ypos / 2);

                if (enableFlame)
                {
                }

                DrawPNG(this->m4a1_png, drawPosX, drawPosY, 1, _m);
            }
            else if (weapon_current == WeaponEnum::AWP)
            {
                drawPosX = int(weapon_Xcof * 2);
                drawPosY = int(weapon_Ypos / 2);

                if (enableFlame)
                {
                }

                //draw scope/draw sniper:
                if (weapons[(int)weapon_current]->openScope)
                {
                    DrawPNG(this->scope_png, 0, 0);
                }
                else
                {
                    DrawPNG(this->awp_png, drawPosX, drawPosY, 1, _m);
                }
            }
            else if (weapon_current == WeaponEnum::ZombieEvilClaw)
            {
                drawPosX = int(weapon_Xcof * 2);
                drawPosY = int(weapon_Ypos / 2);

                DrawPNG(this->zombie_hand_png, drawPosX, drawPosY, 1, _m);
            }
            else if (weapon_current == WeaponEnum::KNIFE)
            {
                drawPosX = int(weapon_Xcof * 2);
                drawPosY = int(weapon_Ypos / 2);

                DrawPNG(this->knife_png, drawPosX, drawPosY, 1, _m);
            }

        }

        if (enableBloodBar)
        {
            for (size_t i = 0; i < 10; i++)
            {
                for (size_t j = 0; j < 70; j++)
                {
                    if (this->playerHealth <= 25)
                    {
                        Draw(bloodBarPosX + j, bloodBarPosY + i, Pixel(227, 23, 0));
                    }
                    else if (this->playerHealth <= 50)
                    {
                        Draw(bloodBarPosX + j, bloodBarPosY + i, Pixel(255, 97, 0));
                    }
                    else
                    {
                        Draw(bloodBarPosX + j, bloodBarPosY + i, Pixel(0, 201, 87));
                    }
                }
            }

            DrawString(bloodBarPosX + 25, bloodBarPosY + 1, to_string(this->playerHealth), Pixel(255, 255, 255));
        }

        if (weapon_current != WeaponEnum::AWP)
        {
            DrawPNG(this->frontSight, 0, 0, 1);
        }
    }

    vi2d get_random_available_pos()
    {
        vector<vi2d> positions;
        for (size_t i = 0; i < mapHeight; i++)
        {
            for (size_t j = 0; j < mapWidth; j++)
            {
                //dont add walls:
                if (map[mapWidth * i + j] != L'#')
                {
                    positions.push_back(vi2d(j, i));
                }
            }
        }
        uint randomIndex = Random::Range(0, positions.size() - 1);
        return positions[randomIndex];
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
    Pixel shade(int mapPosX, int mapPosY, CellSide side, Color24 pixelColor, float sampleX, float sampleY, float distance, float _m, const vector<PointLight*>& pointLights)
    {
        Pixel pixel(pixelColor.r, pixelColor.g, pixelColor.b);

        //night:
        if (night)
        {
            switch (side)
            {
            case CellSide::Top:
            case CellSide::Bottom:
                pixel.r = pixel.r * 0.25f;
                pixel.g = pixel.g * 0.25f;
                pixel.b = pixel.b * 0.25f;
                break;
            }
        }

        if (side == CellSide::Top)
        {
            return pixel;

            //TODO:sky box
            short att = spriteWall->SampleColour(sampleX, sampleY);

            ConsoleColor foreColor = (ConsoleColor)(att & 0x000F);
            ConsoleColor backColor = (ConsoleColor)((att & 0x00F0) / 16);

            Color24 pixelColor = palette[foreColor];
            UNUSED(backColor);
            pixel = Pixel(pixelColor.r, pixelColor.g, pixelColor.b);
        }
        else if (side == CellSide::Bottom)
        {
            if (lightGround)
            {
                _m = 0.0f;
                vf2d pixelPos = vf2d(mapPosX + sampleX, mapPosY + sampleY);

                for (PointLight* pointLight : pointLights)
                {
                    //float distanceToPointLight = (pointLight->gameObject->transform->position - pixelPos).mag();
                    float distanceToPointLight = (pointLight->gameObject->transform->position - pixelPos).mag2();
                    _m += max(0.0f, 1.0f - min(distanceToPointLight / (pointLight->range * 1.5f), 1.0f));
                }
                pixel.r = fuck_std::clamp<float>(pixel.r * (1 + _m), 0, 255);
                pixel.g = fuck_std::clamp<float>(pixel.g * (1 + _m), 0, 255);
                pixel.b = fuck_std::clamp<float>(pixel.b * (1 + _m), 0, 255);

                //this->groundPixelIndex++;
            }
            else
            {

            }

            //fog:
            if (enableFog)
            {
                Color24 fogColor = defaultFogColor;
                float fDistance = 1.0f;
                float fog = 0.0f;

                if (_m > 0)
                {
                    fogColor = Color24(255, 255, 255);
                    fDistance = 1.0f - std::min(distance / 10.0f, 1.0f);
                }
                else
                {
                    fDistance = 1.0f - std::min(distance / 7.5f, 1.0f);
                }

                fog = 1.0 - fDistance;

                pixel.r = fuck_std::clamp<float>(fDistance * pixel.r + fog * fogColor.r, 0, 255);
                pixel.g = fuck_std::clamp<float>(fDistance * pixel.g + fog * fogColor.g, 0, 255);
                pixel.b = fuck_std::clamp<float>(fDistance * pixel.b + fog * fogColor.b, 0, 255);
            }

            //distance:
            if (renderBasedOnDistance)
            {
                float _d = 1.0f;

                _d = 1.0f - std::min(distance / depth, 0.25f);

                pixel.r = fuck_std::clamp<float>(pixel.r * _d, 0, 255);
                pixel.g = fuck_std::clamp<float>(pixel.g * _d, 0, 255);
                pixel.b = fuck_std::clamp<float>(pixel.b * _d, 0, 255);
            }
        }
        else
        {
            pixel.r = fuck_std::clamp<float>(pixel.r * (1 + _m), 0, 255);
            pixel.g = fuck_std::clamp<float>(pixel.g * (1 + _m), 0, 255);
            pixel.b = fuck_std::clamp<float>(pixel.b * (1 + _m), 0, 255);

            //fog:
            if (enableFog)
            {
                Color24 fogColor = defaultFogColor;
                float fDistance = 1.0f;
                float fog = 0.0f;

                if (_m > 0)
                {
                    fogColor = Color24(255, 255, 255);
                    fDistance = 1.0f - std::min(distance / 60, 1.0f);
                    fog = 1.0 - fDistance;
                }
                else
                {
                    fDistance = 1.0f - std::min(distance / 15, 1.0f);
                    fog = 1.0 - fDistance;
                }

                pixel.r = fuck_std::clamp<float>(fDistance * pixel.r + fog * fogColor.r, 0, 255);
                pixel.g = fuck_std::clamp<float>(fDistance * pixel.g + fog * fogColor.g, 0, 255);
                pixel.b = fuck_std::clamp<float>(fDistance * pixel.b + fog * fogColor.b, 0, 255);
            }

            //distance:
            if (renderBasedOnDistance)
            {
                float _d = 1.0f;

                if (_m > 0)
                {
                    _d = 1.0f - std::min(distance / (depth * 4), 0.4f);
                }
                else
                {
                    _d = 1.0f - std::min(distance / depth, 0.4f);
                }

                pixel.r = fuck_std::clamp<float>(pixel.r * _d, 0, 255);
                pixel.g = fuck_std::clamp<float>(pixel.g * _d, 0, 255);
                pixel.b = fuck_std::clamp<float>(pixel.b * _d, 0, 255);
            }
        }

        return pixel;
    }

    //you can add shader code here:
    Pixel shade_object(int mapPosX, int mapPosY, float sampleX, float sampleY, Color24 pixelColor, float distance, float _m)
    {
        Pixel pixel(pixelColor.r, pixelColor.g, pixelColor.b);

        //point lights:
        pixel.r = fuck_std::clamp<float>(pixel.r * (1 + _m), 0, 255);
        pixel.g = fuck_std::clamp<float>(pixel.g * (1 + _m), 0, 255);
        pixel.b = fuck_std::clamp<float>(pixel.b * (1 + _m), 0, 255);

        //fog:
        if (enableFog)
        {
            Color24 fogColor = defaultFogColor;
            float fDistance = 1.0f;
            float fog = 0.0f;

            if (_m > 0)
            {
                fogColor = Color24(255, 255, 255);
                fDistance = 1.0f - std::min(distance / 60, 1.0f);
                fog = 1.0 - fDistance;
            }
            else
            {
                fDistance = 1.0f - std::min(distance / 15, 1.0f);
                fog = 1.0 - fDistance;
            }

            pixel.r = fuck_std::clamp<float>(fDistance * pixel.r + fog * fogColor.r, 0, 255);
            pixel.g = fuck_std::clamp<float>(fDistance * pixel.g + fog * fogColor.g, 0, 255);
            pixel.b = fuck_std::clamp<float>(fDistance * pixel.b + fog * fogColor.b, 0, 255);
        }

        //distance:
        if (renderBasedOnDistance)
        {
            float _d = 1.0f;

            if (_m > 0)
            {
                _d = 1.0f - std::min(distance / (depth * 4), 0.4f);
            }
            else
            {
                _d = 1.0f - std::min(distance / depth, 0.4f);
            }

            pixel.r = fuck_std::clamp<float>(pixel.r * _d, 0, 255);
            pixel.g = fuck_std::clamp<float>(pixel.g * _d, 0, 255);
            pixel.b = fuck_std::clamp<float>(pixel.b * _d, 0, 255);
        }

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

        this->mouseSpeed = database->GetFloat(L"mouseSpeed", 0.05f);
        this->disableBGM = database->GetBool(L"disableBGM", false);
        this->muteAll = database->GetBool(L"muteAll", false);

        int graphicalQualityLevel = database->GetInt(L"GQ", 2);
        switch (graphicalQualityLevel)
        {
        case 1:
            this->graphicalQuality = GraphicalQuality::Low;
            break;
        case 2:
            this->graphicalQuality = GraphicalQuality::Middle;
            break;
        case 3:
            this->graphicalQuality = GraphicalQuality::Hign;
            break;
        case 4:
            this->graphicalQuality = GraphicalQuality::Ultra;
            break;
        default:
            this->graphicalQuality = GraphicalQuality::Middle;
            break;
        }

        switch (this->graphicalQuality)
        {
        case GraphicalQuality::Low:
            this->night = true;
            this->enableFog = false;
            this->renderBasedOnDistance = false;
            this->lightGround = false;
            break;
        case GraphicalQuality::Middle:
        case GraphicalQuality::Hign:
        case GraphicalQuality::Ultra:
            this->night = true;
            this->enableFog = true;
            this->renderBasedOnDistance = true;
            this->lightGround = true;
            break;
        }

        int netWorkRole = database->GetInt(L"networkRole", 0);
        switch (netWorkRole)
        {
        case 1:
            this->networkType = NetworkType::Host;
            this->netIPAddress = "127.0.0.1"; //如果无法连接本机IP则需要修改Windows权限
            this->sAppName = "PixelFPS Demo2 Host";
            break;
        case 2:
            this->networkType = NetworkType::Client;
            this->netIPAddress = String::WstringToString(database->GetString(L"ip", L""));
            this->sAppName = "PixelFPS Demo2 Client";
            break;
        }

        this->gameMode = (GameMode)database->GetInt(L"gameMode", 1);
        wstring soloWeaponText = database->GetString(L"soloWeapon", L"de");
        if (soloWeaponText == L"de")
        {
            this->soloWeapon = WeaponEnum::DESERT_EAGLE;
        }
        else if (soloWeaponText == L"ak47")
        {
            this->soloWeapon = WeaponEnum::AK47;
        }
        else if (soloWeaponText == L"aek971")
        {
            this->soloWeapon = WeaponEnum::AEK_971;
        }
        else if (soloWeaponText == L"awp")
        {
            this->soloWeapon = WeaponEnum::AWP;
        }
        else if (soloWeaponText == L"m4a1")
        {
            this->soloWeapon = WeaponEnum::M4A1;
        }
        else
        {
            this->soloWeapon = WeaponEnum::DESERT_EAGLE;
        }

        delete database;
    }

    PixelFPSDemo2() : GM(GameManager::Global.GetInstance())
    {
        this->sAppName = "PixelFPS Demo2";
    }

    // Called once on application startup, use to load your resources
    bool OnUserCreate() override
    {
        //multiplayer mode:
        if (this->networkType == NetworkType::Host)
        {
            //open server:
            this->server = new FPSServer(net_port);
            this->server->isHost = true;
            this->server->Start();
            //create new thread for msg handeling.
            this->serverThread = new thread(serverTask, this->server);

            //connect to server:
            bool conn_suc = Connect(this->netIPAddress, this->net_port);
        }
        else if (this->networkType == NetworkType::Client)
        {
            //connect to server:
            bool conn_suc = Connect(this->netIPAddress, this->net_port);
        }

        //Redirect:
        window = Window(this->__gameWindow);

        //create map:
        map += L"################################";
        map += L"#..............................#";
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
        map += L"###..........########....#######";
        map += L"####............######.........#";
        map += L"#..............................#";
        map += L"#...........#####.......##..####";
        map += L"#..............##..............#";
        map += L"#.......##.....##.......#......#";
        map += L"#.......##.....##......##......#";
        map += L"#..............##..............#";
        map += L"###............###.............#";
        map += L"##.............###.............#";
        map += L"#..............###...........###";
        map += L"#..............................#";
        map += L"#..............................#";
        map += L"#..............##..............#";
        map += L"#...........................####";
        map += L"#..............##..............#";
        map += L"################################";

        this->de_png = new olc::Sprite(Resources::GetPath("../../", "res/png/", "deagle.png"));
        this->ak_png = new olc::Sprite(Resources::GetPath("../../", "res/png/", "ak.png"));
        this->aek_png = new olc::Sprite(Resources::GetPath("../../", "res/png/", "ak.png"));
        this->awp_png = new olc::Sprite(Resources::GetPath("../../", "res/png/", "awp.png"));
        this->m4a1_png = new olc::Sprite(Resources::GetPath("../../", "res/png/", "m4a1.png"));
        this->scope_png = new olc::Sprite(Resources::GetPath("../../", "res/png/", "scope.png"));
        this->GSG9_png = new olc::Sprite(Resources::GetPath("../../", "res/png/", "player.png"));
        this->Zombie_png = new olc::Sprite(Resources::GetPath("../../", "res/png/", "zombie.png"));
        this->frontSight = new olc::Sprite(Resources::GetPath("../../", "res/png/", "crosshair.png"));
        this->zombie_hand_png = new olc::Sprite(Resources::GetPath("../../", "res/png/", "zombie_hand.png"));
        this->knife_png = new olc::Sprite(Resources::GetPath("../../", "res/png/", "knife.png"));

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

        //load audios:
        if (!muteAll)
        {
            this->bgm = Resources::Load<Audio>(L"../../", L"res/audios/", L"Silent Hill 2 OST - Laura Plays The Piano.mp3");
            this->bgm2 = Resources::Load<Audio>(L"../../", L"res/audios/", L"Silent Hill 2 OST - True.mp3");
            this->explosionPool = Resources::Load<AudioPool>(L"../../", L"res/audios/", L"548_Effect.Explosion.wav.mp3");
            this->fireBallPool = Resources::Load<AudioPool>(L"../../", L"res/audios/", L"560_Weapon.Rocket.Fire.wav.mp3");
            this->aekBulletPool = Resources::Load<AudioPool>(L"../../", L"res/audios/", L"aek_shot.mp3");
            this->dePool = Resources::Load<AudioPool>(L"../../", L"res/audios/weapons/", L"deagle-1.wav");
            this->ak47Pool = Resources::Load<AudioPool>(L"../../", L"res/audios/weapons/", L"ak47-1.wav");
            this->m4a1Pool = Resources::Load<AudioPool>(L"../../", L"res/audios/weapons/", L"m4a1_unsil-1.wav");
            this->awpPool = Resources::Load<AudioPool>(L"../../", L"res/audios/weapons/", L"awp1.wav");

            //play bgm:
            this->bgm2->SetVolume(MCI_MAX_VOLUME / 3);
            if (!disableBGM)
            {
                this->bgm2->Play(false, false);
            }
        }

        vf2d lampSize = vf2d(0.25, 1.25f);

        //add lamps:
        GameObject* lamp1 = new GameObject();
        lamp1->transform->position = vf2d(8.5f, 8.5f);
        auto s1 = lamp1->AddComponent<SpriteRenderer>();
        s1->sprite = this->spriteLamp;
        s1->ObjectSize = lampSize;
        lamp1->AddComponent<PointLight>(5.0f);

        GameObject* lamp2 = new GameObject();
        lamp2->transform->position = vf2d(7.5f, 7.5f);
        auto s2 = lamp2->AddComponent<SpriteRenderer>();
        s2->sprite = this->spriteLamp;
        s2->ObjectSize = lampSize;
        lamp2->AddComponent<PointLight>(5.0f);

        GameObject* lamp3 = new GameObject();
        lamp3->transform->position = vf2d(10.5f, 3.5f);
        auto s3 = lamp3->AddComponent<SpriteRenderer>();
        s3->sprite = this->spriteLamp;
        s3->ObjectSize = lampSize;
        lamp3->AddComponent<PointLight>(5.0f);

        //add players:
        //GameObject* player1 = new GameObject();
        //player1->transform->position = vf2d(8.5f, 14.7f);
        //PNGRenderer* player1_pngRenderer = player1->AddComponent<PNGRenderer>();
        //player1_pngRenderer->sprite = this->GSG9_png;
        //player1_pngRenderer->ObjectSize = vf2d(1.5f, 0.7f);

        //add zombies:
        //GameObject* zombie1 = new GameObject();
        //zombie1->transform->position = vf2d(6, 16);
        //PNGRenderer* zombie1_pngRenderer = zombie1->AddComponent<PNGRenderer>();
        //zombie1_pngRenderer->sprite = this->Zombie_png;
        //zombie1_pngRenderer->ObjectSize = vf2d(1.5f, 0.7f);

        if (networkType != NetworkType::Client)
        {
            ChangeGameMode(this->gameMode);
        }

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

        //quit game:
        if (GetKey(Key::ESCAPE).bPressed)
        {
            return false;
        }

        //multiplayer mode: client code:
        if (this->networkType != NetworkType::None)
        {
            //update client:
            if (IsConnected())
            {
                //receive from server:
                while (!Incoming().empty())
                {
                    auto msg = Incoming().pop_front().msg;

                    switch (msg.header.id)
                    {
                    case(NetworkMessage::Client_Accepted):
                    {
                        std::cout << "Server accepted client - you're in!\n";
                        olc::net::message<NetworkMessage> msg;
                        msg.header.id = NetworkMessage::Client_RegisterWithServer;

                        PlayerNetData defaultPlayerNetData;

                        defaultPlayerNetData.health = playerHealth;
                        defaultPlayerNetData.posX = playerX;
                        defaultPlayerNetData.posY = playerY;

                        //msg << defaultPlayerNetData;
                        msg.AddBytes(defaultPlayerNetData.Serialize());
                        Send(msg);
                        break;
                    }
                    case(NetworkMessage::Client_AssignID):
                    {
                        // Server is assigning us OUR id
                        //msg >> playerID;
                        playerID = msg.GetUInt32();
                        std::cout << "Assigned Client ID = " << playerID << "\n";

                        //tell server game mode if you are host:
                        if (networkType == NetworkType::Host)
                        {
                            olc::net::message<NetworkMessage> msg;
                            msg.header.id = NetworkMessage::Game_HostChooseMode;
                            HostChoose hostChoose;
                            hostChoose.uniqueID = playerID;
                            hostChoose.gameMode = (int)this->gameMode;
                            hostChoose.soloWeapon = (int)this->soloWeapon;
                            msg.AddBytes(hostChoose.Serialize());
                            Send(msg);
                        }

                        break;
                    }
                    case(NetworkMessage::Game_AddPlayer):
                    {
                        PlayerNetData desc;
                        //msg >> desc;
                        desc.Deserialize(msg.body);

                        mapObjects.insert_or_assign(desc.uniqueID, desc);

                        //clone player, dont add repeat player, and dont add ourself.
                        if (networkObjects.count(desc.uniqueID) == 0 && desc.uniqueID != playerID)
                        {
                            GameObject* newPlayer = new GameObject();
                            newPlayer->transform->position = vf2d(desc.posX, desc.posY);

                            PNGRenderer* newPlayer_pngRenderer = newPlayer->AddComponent<PNGRenderer>();
                            newPlayer_pngRenderer->sprite = this->GSG9_png;
                            newPlayer_pngRenderer->ObjectSize = vf2d(1.5f, 0.7f);

                            networkObjects.insert_or_assign(desc.uniqueID, newPlayer);

                            //add cache:
                            networkObjectPositions.insert_or_assign(desc.uniqueID, vf2d(desc.posX, desc.posY));
                        }

                        if (desc.uniqueID == playerID)
                        {
                            // Now we exist in game world
                            waitingForConnection = false;
                        }
                        break;
                    }
                    case(NetworkMessage::Game_RemovePlayer):
                    {
                        uint32_t nRemovalID = 0;
                        //msg >> nRemovalID;
                        nRemovalID = msg.GetUInt32();
                        mapObjects.erase(nRemovalID);

                        //delete player:
                        if (networkObjects.count(nRemovalID) > 0)
                        {
                            delete networkObjects[nRemovalID];
                            networkObjects.erase(nRemovalID);
                            networkObjectPositions.erase(nRemovalID);
                        }

                        break;
                    }
                    //we wont receive msg, which sent from our client.
                    case(NetworkMessage::Game_UpdatePlayer):
                    {
                        PlayerNetData desc;
                        //msg >> desc;
                        desc.Deserialize(msg.body);

                        mapObjects.insert_or_assign(desc.uniqueID, desc);

                        //if dont exsists, add it.
                        if (networkObjects.count(desc.uniqueID) == 0)
                        {
                            GameObject* newPlayer = new GameObject();

                            newPlayer->transform->position = vf2d(desc.posX, desc.posY);

                            PNGRenderer* newPlayer_pngRenderer = newPlayer->AddComponent<PNGRenderer>();
                            newPlayer_pngRenderer->sprite = this->GSG9_png;
                            newPlayer_pngRenderer->ObjectSize = vf2d(1.5f, 0.7f);

                            networkObjects.insert_or_assign(desc.uniqueID, newPlayer);

                            //add cache:
                            networkObjectPositions.insert_or_assign(desc.uniqueID, vf2d(desc.posX, desc.posY));

                            //bullet:
                            networkBullets.insert_or_assign(desc.uniqueID, vector<GameObject*>());
                        }

                        //sync object position:
                        //networkObjects[desc.uniqueID]->transform->position = vf2d(desc.posX, desc.posY);
                        networkObjectPositions[desc.uniqueID] = vf2d(desc.posX, desc.posY);

                        break;
                    }
                    case NetworkMessage::Game_BulletHitOther:
                    {
                        BulletHitInfo info;
                        info.Deserialize(msg.body);
                        //behit
                        if (info.otherPlayerID == playerID)
                        {
                            this->playerHealth -= info.damage;

                            //set behit ui:
                            this->netBeHit = true;

                            //dead:
                            if (this->playerHealth <= 0)
                            {
                                //reset behit ui:
                                this->netBeHit = false;
                                this->beHitEffectTimer = 0.0f;

                                olc::net::message<NetworkMessage> dead_msg;
                                dead_msg.header.id = NetworkMessage::Game_ImDead;
                                ImDead imdead;
                                imdead.ID = playerID;
                                imdead.killerID = info.myID;
                                dead_msg.AddBytes(imdead.Serialize());
                                Send(dead_msg);
                            }
                        }
                    }
                    break;
                    case NetworkMessage::Game_ImDead:
                    {
                        ImDead imdead;
                        imdead.Deserialize(msg.body);
                        //玩家死亡后禁用其游戏物体:
                        networkObjects[imdead.ID]->active = false;

                        //if I kill the player:
                        if (imdead.killerID == playerID)
                        {
                            make_kill_hud();
                        }
                    }
                    break;
                    case NetworkMessage::Game_IRespawn:
                    {
                        IRespawn iRespawn;
                        iRespawn.Deserialize(msg.body);
                        //玩家复活后显示其游戏物体:
                        networkObjects[iRespawn.uniqueID]->active = true;
                        //设置坐标防止复活后还移动插值:
                        networkObjects[iRespawn.uniqueID]->transform->position.x = iRespawn.posX;
                        networkObjects[iRespawn.uniqueID]->transform->position.y = iRespawn.posY;
                    }
                    break;
                    case NetworkMessage::Game_HostChooseMode:
                    {
                        HostChoose choose;
                        choose.Deserialize(msg.body);

                        //set game mode:
                        this->gameMode = (GameMode)choose.gameMode;
                        this->soloWeapon = (WeaponEnum)choose.soloWeapon;
                        ChangeGameMode(this->gameMode);
                    }
                    break;
                    case NetworkMessage::Game_Zombie_Infect:
                    {
                        ZombieInfect zombieInfect;
                        zombieInfect.Deserialize(msg.body);
                        //如果自己被感染了:
                        if (zombieInfect.targetID == this->playerID)
                        {
                            TurnToZombie();
                        }
                        break;
                    }
                    }
                }

                if (waitingForConnection)
                {
                    Clear(olc::DARK_BLUE);
                    DrawString({ 10, 10 }, "Waiting To Connect...", olc::WHITE);
                    return true;
                }

                //sync our position to other clients:
                mapObjects[playerID].posX = playerX;
                mapObjects[playerID].posY = playerY;

                //sync bullets positon:
                for (int i = 0; i < mapObjects[playerID].bullets.size(); i++)
                {
                    mapObjects[playerID].bullets[i].x = myBullets[i]->transform->position.x;
                    mapObjects[playerID].bullets[i].y = myBullets[i]->transform->position.y;
                }

                //sync health:
                mapObjects[playerID].health = this->playerHealth;

                //sync zs_playerType:
                mapObjects[playerID].zs_PlayerType = this->zsPlayerType;

                //send to server:
                olc::net::message<NetworkMessage> msg;
                msg.header.id = NetworkMessage::Game_UpdatePlayer;
                //msg << mapObjects[playerID];
                msg.AddBytes(mapObjects[playerID].Serialize());
                Send(msg);
            }
        }

        //other player position lerp:
        if (networkType != NetworkType::None)
        {
            for (const auto& p : networkObjectPositions)
            {
                //lerp object position:
                networkObjects[p.first]->transform->position.x =
                    fuck_std::lerpf(networkObjects[p.first]->transform->position.x, p.second.x, deltaTime * 2.0f);

                networkObjects[p.first]->transform->position.y =
                    fuck_std::lerpf(networkObjects[p.first]->transform->position.y, p.second.y, deltaTime * 2.0f);
            }
        }

        //update game:
        update_network(deltaTime);

        //update audio:
        update_audio();

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
                    fuck_std::debug_output_vector2(::vi2d(simple_ai->x, simple_ai->y));
                }
            }
        }

        //update physics & collision detect:
        update_world(deltaTime);

        //Player Dead:
        if (this->playerHealth <= 0)
        {
            Clear(Pixel(0, 0, 255));
            DrawString(10, 100, "You are dead! Press 'R' to respawn.");

            if (GetKey(Key::R).bPressed)
            {
                Respawn();
            }

            return true;
        }

        HWND foreWindow = GetForegroundWindow();
        if (foreWindow == this->__gameWindow)
        {
            receive_user_input(deltaTime);
        }

        //vision:
        if (this->gameMode == GameMode::MultiPlayer_ZombieEscapeMode)
        {
            if (zsPlayerType == ZS_PlayerType::Human)
            {
                enableFog = true;
                renderBasedOnDistance = true;
                night = true;
            }
            else if (zsPlayerType == ZS_PlayerType::Zombie)
            {
                enableFog = false;
                renderBasedOnDistance = true;
                night = true;
            }
        }

        render_world(deltaTime);

        render_hud(deltaTime);

        draw_behit_hud(deltaTime);

        draw_hit_hud(deltaTime);

        draw_kill_hud(deltaTime);

        //remove garbage game objects:
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
        if (networkType == NetworkType::Host)
        {
            this->serverThread->detach();
            delete this->serverThread;
            //delete this->server; //disable this line avoid exception.
        }

        delete this->de_png;
        delete this->ak_png;
        delete this->aek_png;
        delete this->awp_png;
        delete this->m4a1_png;
        delete this->scope_png;
        delete this->GSG9_png;
        delete this->Zombie_png;
        delete this->frontSight;
        delete this->zombie_hand_png;
        delete this->knife_png;

        delete this->spriteWall;
        delete this->spriteLamp;
        delete this->spriteFireBall;
        delete this->spriteExplosion;
        delete this->spriteFlower;
        delete this->sptireWeapon_aek;

        if (!muteAll)
        {
            delete this->bgm;
            delete this->bgm2;
            delete this->explosionPool;
            delete this->fireBallPool;
            delete this->aekBulletPool;
            delete this->dePool;
            delete this->ak47Pool;
            delete this->m4a1Pool;
            delete this->awpPool;
        }

        delete[] this->depthBuffer;

        return true;
    }
};