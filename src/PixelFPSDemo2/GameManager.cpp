#include "GameManager.hpp"
#include "GameObject.hpp"
#include "Transform.hpp"
#include "tool.hpp"

void GameManager::AddGameObject(GameObject* go)
{
    go->id = startId++;
    gameObjects.insert_or_assign(go->id, go);
}

void GameManager::RemoveGameObject(GameObject* go)
{
    gameObjects.erase(go->id);
}

void GameManager::Update(float deltaTime)
{
    for (const auto& item : gameObjects)
    {
        GameObject* go = item.second;
        Transform* t = go->GetComponent<Transform>();
        debug_output_line(to_wstring(t->position.x) + L" " + to_wstring(t->position.y));
    }
}
