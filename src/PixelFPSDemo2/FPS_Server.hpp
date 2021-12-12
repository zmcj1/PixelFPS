#pragma once

#include "NetworkMessage.hpp"
//#include "olcPGEX_Network.h"

#include <iostream>
#include <unordered_map>

using namespace std;
using namespace olc;
//using namespace olc::net;

//class FPSServer :public server_interface<NetworkMessage>
//{
//private:
//    std::unordered_map<uint32_t, PlayerNetData> playerNetDataDict;
//    std::vector<uint32_t> garbageIDs;
//
//public:
//    FPSServer(uint16_t port) : server_interface<NetworkMessage>(port)
//    {
//    }
//
//    bool OnClientConnect(std::shared_ptr<olc::net::connection<NetworkMessage>> client) override
//    {
//        // For now we will allow all 
//        return true;
//    }
//
//    void OnClientValidated(std::shared_ptr<olc::net::connection<NetworkMessage>> client) override
//    {
//        // Client passed validation check, so send them a message informing
//        // them they can continue to communicate
//        olc::net::message<NetworkMessage> msg;
//        msg.header.id = NetworkMessage::Client_Accepted;
//        client->Send(msg);
//    }
//
//    void OnClientDisconnect(std::shared_ptr<olc::net::connection<NetworkMessage>> client) override
//    {
//        if (client)
//        {
//            if (playerNetDataDict.find(client->GetID()) == playerNetDataDict.end())
//            {
//                // client never added to roster, so just let it disappear
//            }
//            else
//            {
//                auto& pd = playerNetDataDict[client->GetID()];
//                std::cout << "[UNGRACEFUL REMOVAL]:" + std::to_string(pd.uniqueID) + "\n";
//                playerNetDataDict.erase(client->GetID());
//                garbageIDs.push_back(client->GetID());
//            }
//        }
//    }
//};