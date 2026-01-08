#include "renderer.hpp"

#include "ECS.hpp"
#include "components/position.hpp"
#include "components/sprite.hpp"
#include "enums/entity_types.hpp"

void AssetManager::init()
{
    _textures.reserve(static_cast<size_t>(
        entity::Type::kBullet));  // kBullet is the last element, hence the biggest index.
    _textures.emplace_back(PLAYER_TEXTURE_FILEPATH, 0.2);
}

const gui::Texture& AssetManager::getTexture(entity::Type id) const
{
    return _textures[static_cast<size_t>(id)];
}

namespace systems {

Renderer::Renderer()
    : ASystem(rtecs::DynamicBitSet()),
      _closing(false)
{
    InitWindow(1920, 1080, "Renderer");
    _assetManager.init();
}

void Renderer::apply(rtecs::ECS& ecs)
{
    if (WindowShouldClose()) {
        if (_closing) {
            return;
        }
        CloseWindow();
        _closing = true;
        return;
    }
    BeginDrawing();
    ClearBackground({0, 255, 0, 255});
    for (const auto& [sprite, pos] : ecs.view<components::Sprite, components::Position>()) {
        const gui::Texture& tex = _assetManager.getTexture(sprite.type);
        DrawTextureEx(tex.getTexture(), {pos.x, pos.y}, 0.0f, tex.getScale(), WHITE);
    }
    EndDrawing();
}

}  // namespace systems
