#include "GameObject.hpp"

GameObject::GameObject()
{
    GameManager::Global.GetInstance().AddGameObject(this);
    this->name = "GameObject";
    active = true;
    parent = nullptr;
}

GameObject::GameObject(const string& name)
{
    GameManager::Global.GetInstance().AddGameObject(this);
    this->name = name;
    active = true;
    parent = nullptr;
}