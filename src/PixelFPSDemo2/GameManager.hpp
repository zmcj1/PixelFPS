#pragma once

#include "Singleton.hpp"
#include <unordered_map>

class GameObject;

using namespace MinConsoleNative;
using namespace std;

class GameManager
{
public:
    static Singleton<GameManager> Global;

private:
    int startId = 0;
    unordered_map<int, GameObject*> gameObjects;

public:
    void AddGameObject(GameObject* go);
};