#include "rteng.hpp"

#include "behaviour.hpp"

namespace rteng {

void GameEngine::destroyEntity(const rtecs::types::EntityID& id) const { _ecs->destroyEntity(id); }

void GameEngine::setGameState(const size_t& newState) { _gameState = newState; }

size_t GameEngine::getGameState() const { return _gameState; }

void GameEngine::setMenuState(const size_t& newState) { _menuState = newState; }

size_t GameEngine::getMenuState() const { return _menuState; }

void GameEngine::runOnce(const double dt) const
{
    auto behaviours = _ecs->group<components::Behaviour>();

    behaviours.apply([&dt](const rtecs::types::EntityID&, components::Behaviour& c) {
        if (!c.instance) {
            return;
        }
        if (!c.started) {
            c.instance->Start();
            c.started = true;
        }
        c.instance->Update(dt);
    });
    _ecs->applyAllSystems();
}

}  // namespace rteng
