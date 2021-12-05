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
public:
    WeaponEnum weapon_enum;
    WeaponType weapon_type;
    OLCSprite* olcSprite;

    Weapon()
    {
        this->weapon_enum = WeaponEnum::UNKNOWN;
        this->weapon_type = WeaponType::Unknown;
        this->olcSprite = nullptr;
    }


};