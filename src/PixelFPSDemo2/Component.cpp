#include "Component.hpp"
#include "GameObject.hpp"

const std::size_t Component::Type = std::hash<std::string>()(NAME_OF(Component));