#pragma once

#include "OLCSprite.hpp"

enum class WeaponEnum
{
    UNKNOWN = 0,
    AEK_971 = 1,
    AK47 = 2,
    AWP = 3,
    DESERT_EAGLE = 4,
    KNIFE = 5,
    M4A1 = 6,
    M4A1_S = 7,
};

enum class WeaponType
{
    Unknown = 0,
    Knife = 1,
    Rifle = 2,
    Pistol = 3,
    Shotgun = 4,
    SMG = 5,
    Sniper = 6,
    MachineGun = 7,
    Granade = 8,
};

class Weapon
{
private:
    bool fired = false;
    float fire_timer = 0.0f;

public:
    //properties
    float fire_interval = 0.5f; //just a default value
    float damage = 10.0f;

public:
    WeaponEnum weapon_enum;
    WeaponType weapon_type;
    OLCSprite* olcSprite;
    bool openScope = false;

    Weapon()
    {
        this->weapon_enum = WeaponEnum::UNKNOWN;
        this->weapon_type = WeaponType::Unknown;
        this->olcSprite = nullptr;
    }

    Weapon(WeaponEnum weapon_enum, WeaponType weapon_type, OLCSprite* olcSprite)
    {
        this->weapon_enum = weapon_enum;
        this->weapon_type = weapon_type;
        this->olcSprite = olcSprite;
    }

    void UpdateWeaponTimer(float deltaTime)
    {
        if (fired)
        {
            fire_timer += deltaTime;
            if (fire_timer >= fire_interval)
            {
                fire_timer = 0.0f;
                fired = false;
            }
        }
    }

    bool CanFire()
    {
        if (!fired)
        {
            fired = true;
            return true;
        }
        return false;
    }
};