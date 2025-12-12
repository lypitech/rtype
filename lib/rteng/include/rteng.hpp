#pragma once

#include <memory>
#include <string>
#include <vector>

#include "ASystem.hpp"
#include "ECS.hpp"
#include "Renderer.hpp"

// components
#include "comp/IO.hpp"
#include "comp/Transform.hpp"
// components->monobehaviours
#include "MonoBehaviour.hpp"
#include "comp/Behaviour.hpp"

namespace rteng {

class GameEngine
{
public:
    GameEngine(int screenWidth, int screenHeight, const std::string& title, int fps = 60);

    void run();
    void stop() { _isRunning = false; }

    void registerSystems(std::vector<std::unique_ptr<rtecs::ASystem>> systems);

    template <typename component>
    void registerComponent()
    {
        _ecs->registerComponent<component>();
    }

    template <typename... component>
    void registerEntity(const std::shared_ptr<rteng::behaviour::MonoBehaviour>& mono_behaviour = nullptr)
    {
        const rtecs::EntityID entityId = _ecs->registerEntity<component...>();

        if (!_ecs->hasEntityComponent<comp::Behaviour>(entityId)) {
            return;
        }
        auto& behaviourComponents = _ecs->getComponent<comp::Behaviour>();
        auto& behaviourSparseSet = dynamic_cast<rtecs::SparseSet<comp::Behaviour>&>(behaviourComponents);

        comp::Behaviour behaviourComp;
        behaviourComp.instance = mono_behaviour;
        behaviourComp.started = false;

        behaviourSparseSet.put(entityId, behaviourComp);
    }
    // Rendering System
    // Event System

private:
    graphics::Renderer _renderer;
    std::unique_ptr<rtecs::ECS> _ecs = rtecs::ECS::createWithComponents<comp::Transform, comp::IO, comp::Behaviour>();
    bool _isRunning = false;
};

}  // namespace rteng
