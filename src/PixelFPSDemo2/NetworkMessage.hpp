#pragma once

#include <cstdint>

using namespace std;

enum class NetworkMessage : uint32_t
{
    Unknown = 0,

    Client_Accepted,

    Room_AddNewPlayer,
    Room_RemovePlayer,
    Room_UpdatePlayerNetData,
};

struct PlayerNetData
{
public:
    uint32_t uniqueID = 0;
};