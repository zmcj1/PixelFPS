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

public:
    int startId = 0;
    unordered_map<int, GameObject*> gameObjects;

public:
    void AddGameObject(GameObject* go);

    void RemoveGameObject(GameObject* go);

    void Update(float deltaTime);
};