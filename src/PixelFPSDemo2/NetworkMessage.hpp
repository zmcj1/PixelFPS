﻿#pragma once

#include <vector>
#include <tuple>
#include <cstdint>

using namespace std;

enum class NetworkMessage : uint32_t
{
    //unuse start
    Unknown = 0,
    Server_GetStatus,
    Server_GetPing,
    //unuse end

    Client_Accepted,
    Client_AssignID,
    Client_RegisterWithServer,
    Client_UnregisterWithServer,

    Game_AddPlayer,
    Game_RemovePlayer,
    Game_UpdatePlayer,
};

struct NetBullet
{
public:
    float x;
    float y;

    NetBullet(float x, float y)
    {
        this->x = x;
        this->y = y;
    }
};

struct PlayerNetData
{
public:
    uint32_t uniqueID = 0;

    float posX;
    float posY;

    //todo:
    vector<NetBullet> bullets;

    std::vector<uint8_t> Serialize() const
    {
        std::vector<uint8_t> buffer;

        //module:
        //buffer.resize(buffer.size() + sizeof(DataType));
        //std::memcpy(buffer.data() + buffer.size(), &data, sizeof(DataType));

        size_t pointer = 0;

        buffer.resize(buffer.size() + sizeof(uint32_t));
        std::memcpy(buffer.data() + pointer, &uniqueID, sizeof(uint32_t));
        pointer += sizeof(uint32_t);

        buffer.resize(buffer.size() + sizeof(float));
        std::memcpy(buffer.data() + pointer, &posX, sizeof(float));
        pointer += sizeof(float);

        buffer.resize(buffer.size() + sizeof(float));
        std::memcpy(buffer.data() + pointer, &posY, sizeof(float));
        pointer += sizeof(float);

        //bullets:
        for (const NetBullet bullet : bullets)
        {
            //x:
            buffer.resize(buffer.size() + sizeof(float));
            std::memcpy(buffer.data() + pointer, &bullet.x, sizeof(float));
            pointer += sizeof(float);
            //y:
            buffer.resize(buffer.size() + sizeof(float));
            std::memcpy(buffer.data() + pointer, &bullet.y, sizeof(float));
            pointer += sizeof(float);
        }

        return buffer;
    }

    void Deserialize(const std::vector<uint8_t>& buffer)
    {
        size_t pointer = 0;

        std::memcpy(&uniqueID, buffer.data() + pointer, sizeof(uint32_t));
        pointer += sizeof(uint32_t);

        std::memcpy(&posX, buffer.data() + pointer, sizeof(float));
        pointer += sizeof(float);

        std::memcpy(&posY, buffer.data() + pointer, sizeof(float));
        pointer += sizeof(float);

        //bullets:
    }
};

enum class NetworkType
{
    None = 0,
    Host = 1,   //server + client
    Client = 2, //client
};