#include "renderer.hpp"

#include "components/position.hpp"
#include "components/sprite.hpp"
#include "enums/entity_types.hpp"
#include "rtecs/ECS.hpp"

namespace systems {

Renderer::Renderer(bool& shouldStop)
    : ASystem("Renderer"),
      _closing(shouldStop)
{
    SetTraceLogLevel(LOG_NONE);
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
    ecs.group<components::Sprite, components::Position>().apply(
        [this](const rtecs::types::EntityID&,
               const components::Sprite& sprite,
               const components::Position& pos) {
            const gui::Texture& tex = _assetManager.getTexture(sprite.type);
            DrawTextureEx(tex.getTexture(), {pos.x, pos.y}, 0.0f, tex.getScale(), WHITE);
        });
    EndDrawing();
}

}  // namespace systems
