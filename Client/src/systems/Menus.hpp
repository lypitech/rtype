#pragma once

#include <string_view>

#include "gui/assetManager.hpp"
#include "gui/button.hpp"
#include "network.hpp"
#include "rtecs/systems/ASystem.hpp"
#include "rteng.hpp"

static constexpr std::string_view START_TEXTURE_FILEPATH = "../../Client/assets/buttons/start.png";
static constexpr std::string_view PLAY_TEXTURE_FILEPATH = "../../Client/assets/buttons/play.png";
static constexpr std::string_view CREDITS_TEXTURE_FILEPATH =
    "../../Client/assets/buttons/credits.png";
static constexpr std::string_view BACKGROUND_TEXTURE_FILEPATH =
    "../../Client/assets/background.png";

namespace systems {

/**
 * @brief Renders the menus
 *
 * @note This system is to be registered after `RendererSystem`.
 *
 */
class MenuRenderer : public rtecs::systems::ASystem
{
public:
    MenuRenderer(service::Network& service,
                 rteng::GameEngine& engine);

    void apply(rtecs::ECS& ecs) override;

private:
    service::Network& _service;
    rteng::GameEngine& _engine;
    gui::Texture _texture;
    std::vector<gui::Button> _buttons;
};

}  // namespace systems
