#pragma once

//Inspired by:https://stackoverflow.com/questions/44105058/implementing-component-system-from-unity-in-c

#include "Component.hpp"
#include "Transform.hpp"

#include <string>
#include <functional>
#include <vector>
#include <memory>
#include <algorithm>

using namespace std;

class GameObject
{
public:
    int id;

    string name;

    bool active;

    GameObject* parent;

    vector<GameObject*> childs;

    //refs to Transform:
    Transform* transform;

public:
    GameObject();

    GameObject(const string& name);

    ~GameObject();

private:
    std::vector<std::unique_ptr<Component>> components;

public:
    template<class ComponentType, typename... Args> ComponentType* AddComponent(Args&&... params);

    template<class ComponentType> ComponentType* GetComponent();

    template<class ComponentType> bool RemoveComponent();

    template<class ComponentType> std::vector<ComponentType*> GetComponents();

    template<class ComponentType> int RemoveComponents();
};

//***************
    // GameObject::AddComponent
    // perfect-forwards all params to the ComponentType constructor with the matching parameter list
    // DEBUG: be sure to compare the arguments of this fn to the desired constructor to avoid perfect-forwarding failure cases
    // EG: deduced initializer lists, decl-only static const int members, 0|NULL instead of nullptr, overloaded fn names, and bitfields
    //***************
template< class ComponentType, typename... Args >
ComponentType* GameObject::AddComponent(Args&&... params)
{
    components.emplace_back(std::make_unique<ComponentType>(std::forward<Args>(params)...));
    return static_cast<ComponentType*>(components[components.size() - 1].get());
}

//***************
// GameObject::GetComponent
// returns the first component that matches the template type
// or that is derived from the template type
// EG: if the template type is Component, and components[0] type is BoxCollider
// then components[0] will be returned because it derives from Component
//***************
template< class ComponentType >
ComponentType* GameObject::GetComponent()
{
    for (auto&& component : components)
    {
        if (component->IsClassType(ComponentType::Type))
            return static_cast<ComponentType*>(component.get());
    }

    return std::unique_ptr< ComponentType >(nullptr).get();
}

//***************
// GameObject::RemoveComponent
// returns true on successful removal
// returns false if components is empty, or no such component exists
//***************
template< class ComponentType >
bool GameObject::RemoveComponent()
{
    if (components.empty())
        return false;

    auto index = std::find_if(components.begin(),
        components.end(),
        [classType = ComponentType::Type](auto& component) {
        return component->IsClassType(classType);
    });

    bool success = index != components.end();

    if (success)
        components.erase(index);

    return success;
}

//***************
// GameObject::GetComponents
// returns a vector of pointers to the the requested component template type following the same match criteria as GetComponent
// NOTE: the compiler has the option to copy-elide or move-construct componentsOfType into the return value here
// TODO: pass in the number of elements desired (eg: up to 7, or only the first 2) which would allow a std::array return value,
// except there'd need to be a separate fn for getting them *all* if the user doesn't know how many such Components the GameObject has
// TODO: define a GetComponentAt<ComponentType, int>() that can directly grab up to the the n-th component of the requested type
//***************
template< class ComponentType >
std::vector< ComponentType* > GameObject::GetComponents()
{
    std::vector< ComponentType* > componentsOfType;

    for (auto&& component : components)
    {
        if (component->IsClassType(ComponentType::Type))
            componentsOfType.emplace_back(static_cast<ComponentType*>(component.get()));
    }

    return componentsOfType;
}

//***************
// GameObject::RemoveComponents
// returns the number of successful removals, or 0 if none are removed
//***************
template< class ComponentType >
int GameObject::RemoveComponents()
{
    if (components.empty())
        return 0;

    int numRemoved = 0;
    bool success = false;

    do
    {
        auto index = std::find_if(components.begin(),
            components.end(),
            [classType = ComponentType::Type](auto& component) {
            return component->IsClassType(classType);
        });

        success = index != components.end();

        if (success)
        {
            components.erase(index);
            ++numRemoved;
        }
    }
    while (success);

    return numRemoved;
}
