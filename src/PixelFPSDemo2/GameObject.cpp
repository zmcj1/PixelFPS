#include "GameObject.hpp"
#include "GameManager.hpp"

GameObject::GameObject()
{
    //add self to game_manager:
    GameManager::Global.GetInstance().AddGameObject(this);
    this->name = "GameObject";
    active = true;
    parent = nullptr;
    //add components:
    this->transform = AddComponent<Transform>();
}

GameObject::GameObject(const string& name)
{
    //add self to game_manager:
    GameManager::Global.GetInstance().AddGameObject(this);
    this->name = name;
    active = true;
    parent = nullptr;
    //add components:
    this->transform = AddComponent<Transform>();
}

GameObject::~GameObject()
{
    //remove self from game_manager:
    GameManager::Global.GetInstance().RemoveGameObject(this);
}