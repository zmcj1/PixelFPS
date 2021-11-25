#pragma once

#include <vector>
#include <unordered_map>

using namespace std;

class GameObject;
class Component;

class GameObjectManager
{
public:
    int uniqueIdIndex = 1;

    unordered_map<int, GameObject*> gos;

    void AddGameObject(GameObject* go)
    {
        go->uniqueId = uniqueIdIndex++;
        gos[go->uniqueId] = go;
    }

    void RemoveGameObject(GameObject* go)
    {
        gos.erase(go->uniqueId);
    }

    void UpdateGameObjects(float deltaTime)
    {
        for (const auto& go : gos)
        {
            
        }
    }
};

class GameObject
{
private:
    GameObjectManager& manager;
    unordered_map<int, Component*> components;

public:
    int uniqueId = 0;

    GameObject(GameObjectManager& manager) : manager(manager)
    {
        this->manager.AddGameObject(this);
    }

    ~GameObject()
    {
        this->manager.RemoveGameObject(this);
    }

    void AddComponent(Component* com)
    {

    }

    void RemoveComponent(Component* com)
    {

    }

    void Update(float deltaTime)
    {

    }
};

class Component
{
public:
    int uniqueComId = 0;
    GameObject* go;

    Component(GameObject* go) : go(go)
    {

    }

    ~Component()
    {

    }
};