#include "renderer.hpp"

#include "components/animations.hpp"
#include "components/hitbox.hpp"
#include "components/position.hpp"
#include "components/sprite.hpp"
#include "enums/entity_types.hpp"
#include "rtecs/ECS.hpp"

namespace systems {

Renderer::Renderer(bool& shouldStop,
                   rteng::GameEngine& engine)
    : ASystem("Renderer"),
      _closing(shouldStop),
      _engine(engine)
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
    ClearBackground(GREEN);
    DrawTexture(_assetManager.getBackground(), 0, 0, WHITE);
    ecs.group<components::Sprite, components::Position>().apply(
        [&](const rtecs::types::EntityID& id,
            const components::Sprite& sprite,
            const components::Position& pos) {
            const gui::Texture& tex = _assetManager.getTexture(sprite.type);
            Texture2D rawTex = tex.getTexture();

            Rectangle sourceRec = {0.0f, 0.0f, (float)rawTex.width, (float)rawTex.height};

            const auto animOpt =
                ecs.group<components::Animation>().getEntity<components::Animation>(id);
            if (animOpt.has_value()) {
                const components::Animation& anim = animOpt.value().get();
                sourceRec.x = anim.current_frame * anim.frame_width;
                sourceRec.y = 0.0f;
                sourceRec.width = (float)anim.frame_width;
                sourceRec.height = (float)anim.frame_height;
            }

            float scaleX = (float)tex.getScale();
            float scaleY = (float)tex.getScale();

            const auto hitboxOpt =
                ecs.group<components::Hitbox>().getEntity<components::Hitbox>(id);

            if (hitboxOpt.has_value()) {
                const auto& hb = hitboxOpt.value().get();
                if (sourceRec.width != 0) {
                    scaleX = hb.width / sourceRec.width;
                }
                if (sourceRec.height != 0) {
                    scaleY = hb.height / sourceRec.height;
                }
            }

            Rectangle destRec = {pos.x, pos.y, sourceRec.width * scaleX, sourceRec.height * scaleY};

            DrawTexturePro(rawTex, sourceRec, destRec, {0, 0}, 0.0f, WHITE);
        });
}

}  // namespace systems
