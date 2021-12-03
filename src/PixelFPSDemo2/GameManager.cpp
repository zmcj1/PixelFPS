#include "GameManager.hpp"
#include "GameObject.hpp"

void GameManager::AddGameObject(GameObject* go)
{
    go->id = startId++;
    gameObjects.insert_or_assign(go->id, go);
}