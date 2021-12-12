#pragma once

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

struct PlayerNetData
{
public:
    uint32_t uniqueID = 0;

    float posX;
    float posY;
};

enum class NetworkType
{
    None = 0,
    Host = 1,   //server + client
    Client = 2, //client
};