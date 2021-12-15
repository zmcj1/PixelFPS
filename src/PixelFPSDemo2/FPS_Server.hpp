#pragma once

#include "NetworkMessage.hpp"
#include "olcPGEX_Network.h"

#include <iostream>
#include <unordered_map>

using namespace std;
using namespace olc;
using namespace olc::net;

class FPSServer : public server_interface<NetworkMessage>
{
private:
    int hostID = 0;
    int gameMode = 0;
    int soloWeapon = 0;

private:
    std::unordered_map<uint32_t, PlayerNetData> playerNetDataDict;
    std::vector<uint32_t> garbageIDs;

public:
    FPSServer(uint16_t port) : server_interface<NetworkMessage>(port)
    {
    }

    bool OnClientConnect(std::shared_ptr<olc::net::connection<NetworkMessage>> client) override
    {
        // For now we will allow all
        return true;
    }

    void OnClientValidated(std::shared_ptr<olc::net::connection<NetworkMessage>> client) override
    {
        // Client passed validation check, so send them a message informing
        // them they can continue to communicate
        olc::net::message<NetworkMessage> msg;
        msg.header.id = NetworkMessage::Client_Accepted;
        client->Send(msg);
    }

    void OnClientDisconnect(std::shared_ptr<olc::net::connection<NetworkMessage>> client) override
    {
        if (client)
        {
            if (playerNetDataDict.find(client->GetID()) == playerNetDataDict.end())
            {
                // client never added to roster, so just let it disappear
            }
            else
            {
                auto& pd = playerNetDataDict[client->GetID()];
                std::cout << "[UNGRACEFUL REMOVAL]:" + std::to_string(pd.uniqueID) + "\n";
                playerNetDataDict.erase(client->GetID());
                garbageIDs.push_back(client->GetID());
            }
        }
    }

    void OnMessage(std::shared_ptr<olc::net::connection<NetworkMessage>> client, olc::net::message<NetworkMessage>& msg) override
    {
        if (!garbageIDs.empty())
        {
            for (auto pid : garbageIDs)
            {
                olc::net::message<NetworkMessage> m;
                m.header.id = NetworkMessage::Game_RemovePlayer;
                //m << pid;
                m.AddUInt32(pid);
                std::cout << "Removing " << pid << "\n";
                MessageAllClients(m);
            }
            garbageIDs.clear();
        }

        switch (msg.header.id)
        {
        case NetworkMessage::Client_RegisterWithServer:
        {
            PlayerNetData desc;
            //msg >> desc;
            desc.Deserialize(msg.body);

            desc.uniqueID = client->GetID();
            playerNetDataDict.insert_or_assign(desc.uniqueID, desc);

            olc::net::message<NetworkMessage> msgSendID;
            msgSendID.header.id = NetworkMessage::Client_AssignID;
            //msgSendID << desc.uniqueID;
            msgSendID.AddUInt32(desc.uniqueID);
            MessageClient(client, msgSendID);

            olc::net::message<NetworkMessage> msgAddPlayer;
            msgAddPlayer.header.id = NetworkMessage::Game_AddPlayer;
            //msgAddPlayer << desc;
            msgAddPlayer.AddBytes(desc.Serialize());
            MessageAllClients(msgAddPlayer);

            //tell this client current game mode:
            //dont send it to host.
            if (this->gameMode != 0 && desc.uniqueID != hostID)
            {
                olc::net::message<NetworkMessage> msgChooseMode;
                msgChooseMode.header.id = NetworkMessage::Game_HostChooseMode;
                HostChoose choose;
                choose.gameMode = this->gameMode;
                choose.uniqueID = this->hostID;
                choose.soloWeapon = this->soloWeapon;
                msgChooseMode.AddBytes(choose.Serialize());
                MessageClient(client, msgChooseMode);
            }

            for (const auto& player : playerNetDataDict)
            {
                olc::net::message<NetworkMessage> msgAddOtherPlayers;
                msgAddOtherPlayers.header.id = NetworkMessage::Game_AddPlayer;
                //msgAddOtherPlayers << player.second;
                msgAddOtherPlayers.AddBytes(player.second.Serialize());
                MessageClient(client, msgAddOtherPlayers);
            }

            break;
        }

        case NetworkMessage::Client_UnregisterWithServer:
        {
            break;
        }

        case NetworkMessage::Game_UpdatePlayer:
        case NetworkMessage::Game_BulletHitOther:
        case NetworkMessage::Game_ImDead:
        case NetworkMessage::Game_IRespawn:
        {
            // Simply bounce update to everyone except incoming client
            MessageAllClients(msg, client);
            break;
        }
        case NetworkMessage::Game_HostChooseMode:
            HostChoose hostChoose;
            hostChoose.Deserialize(msg.body);
            this->gameMode = hostChoose.gameMode;
            this->hostID = hostChoose.uniqueID;
            this->soloWeapon = hostChoose.soloWeapon;
            break;
        }
    }
};
