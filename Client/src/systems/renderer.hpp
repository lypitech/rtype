#pragma once

#include <raylib.h>
#include <memory>

#include "ASystem.hpp"
#include "enums/entity_types.hpp"
#include "gui/texture.hpp"

class AssetManager
{
public:
    AssetManager() = default;

    void init();
    [[nodiscard]] const gui::Texture& getTexture(entity::Type id) const;

private:
    std::vector<gui::Texture> _textures;
    std::vector<std::unique_ptr<Sound>> _sounds;
};

namespace systems {

class Renderer final : public rtecs::ASystem
{
public:
    Renderer();

    void apply(rtecs::ECS& entity) override;

private:
    bool _closing;
    AssetManager _assetManager;
};

}  // namespace systems
