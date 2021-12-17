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
public:
    bool isHost = false;

private:
    int hostID = 0;
    int gameMode = 0;
    int soloWeapon = 0;     //solo mode only

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
            desc.Deserialize(msg.body);

            desc.uniqueID = client->GetID();
            playerNetDataDict.insert_or_assign(desc.uniqueID, desc);

            olc::net::message<NetworkMessage> msgSendID;
            msgSendID.header.id = NetworkMessage::Client_AssignID;
            msgSendID.AddUInt32(desc.uniqueID);
            MessageClient(client, msgSendID);

            //告诉所有玩家，新玩家进来了
            olc::net::message<NetworkMessage> msgAddPlayer;
            msgAddPlayer.header.id = NetworkMessage::Game_AddPlayer;
            msgAddPlayer.AddBytes(desc.Serialize());
            MessageAllClients(msgAddPlayer);

            //如果是纯服务器而且没有选择模式则发送一个默认模式1
            if (!this->isHost && this->gameMode == 0)
            {
                cout << "send to client : survival mode.\n";
                olc::net::message<NetworkMessage> msgChooseMode;
                msgChooseMode.header.id = NetworkMessage::Game_HostChooseMode;
                HostChoose choose;
                choose.gameMode = 1;
                choose.uniqueID = 0;
                choose.soloWeapon = 0;
                msgChooseMode.AddBytes(choose.Serialize());
                MessageClient(client, msgChooseMode);
            }

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

            //告诉该玩家其他玩家的存在
            for (const auto& player : playerNetDataDict)
            {
                //不要再重复发送消息给相同客户端:
                if (player.first == desc.uniqueID) continue;

                olc::net::message<NetworkMessage> msgAddOtherPlayers;
                msgAddOtherPlayers.header.id = NetworkMessage::Game_AddPlayer;
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
