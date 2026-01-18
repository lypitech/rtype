#pragma once

#include <raylib.h>

#include <memory>
#include <vector>

#include "enums/entity_types.hpp"
#include "gui/assetManager.hpp"
#include "gui/texture.hpp"
#include "rtecs/systems/ASystem.hpp"
#include "rteng.hpp"

namespace systems {

class Renderer final : public rtecs::systems::ASystem
{
public:
    Renderer(bool& shouldStop);

    void apply(rtecs::ECS& entity) override;

private:
    bool& _closing;
    gui::AssetManager _assetManager;
};

}  // namespace systems
