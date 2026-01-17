#pragma once

#include <string_view>

#include "gui/assetManager.hpp"
#include "gui/button.hpp"
#include "network.hpp"
#include "rtecs/systems/ASystem.hpp"
#include "rteng.hpp"

namespace client {
struct Lobby;
}

static constexpr std::string_view START_TEXTURE_FILEPATH = "../../Client/assets/buttons/start.png";
static constexpr std::string_view PLAY_TEXTURE_FILEPATH = "../../Client/assets/buttons/play.png";
static constexpr std::string_view CREDITS_TEXTURE_FILEPATH =
    "../../Client/assets/buttons/credits.png";
static constexpr std::string_view BACKGROUND_TEXTURE_FILEPATH =
    "../../Client/assets/background.png";
static constexpr std::string_view FONT_FILEPATH = "../../Client/assets/basic.ttf";
static constexpr std::string_view DYSLEXIC_FONT_FILEPATH = "../../Client/assets/dyslexic.ttf";

static constexpr int BOX_X = 230;
static constexpr int BOX_Y = 300;
static constexpr int BOX_WIDTH = 1000;
static constexpr int BOX_HEIGHT = 700;
static constexpr int IN_BOX_OFFSET_X = 80;
static constexpr int IN_BOX_OFFSET_Y = 40;
static constexpr int BETWEEN_BOX_OFFSET_X = IN_BOX_OFFSET_Y;
static constexpr int BETWEEN_BOX_OFFSET_Y = IN_BOX_OFFSET_Y;
static constexpr int BOX_IN_BOX_ORIGIN_X = BOX_X + IN_BOX_OFFSET_X;
static constexpr int BOX_IN_BOX_ORIGIN_Y = BOX_Y + IN_BOX_OFFSET_Y;
static constexpr int BOX_IN_BOX_WIDTH = (BOX_WIDTH - IN_BOX_OFFSET_X * 2) / 6;
static constexpr int BOX_IN_BOX_HEIGHT = (BOX_HEIGHT - IN_BOX_OFFSET_Y * 3) / 5;

namespace gui {

class LobbyButton
{
public:
    LobbyButton(const uint32_t& roomId,
                Rectangle bounds,
                const std::shared_ptr<Font>& font);

    [[nodiscard]] bool render() const;
    uint32_t getRoomId() const { return _roomId; }

private:
    uint32_t _roomId;
    std::shared_ptr<Font> _font;
    Rectangle _bounds;
};

}  // namespace gui

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
    MenuRenderer(client::Lobby& lobby,
                 service::Network& service,
                 rteng::GameEngine& engine);

    void apply(rtecs::ECS& ecs) override;

private:
    service::Network& _service;
    client::Lobby& _lobby;
    std::vector<gui::LobbyButton> _lobbyButtons;
    std::shared_ptr<Font> _currentFont;
    std::shared_ptr<Font> _basicFont;
    std::shared_ptr<Font> _dyslexicFont;
    rteng::GameEngine& _engine;
    gui::Texture _texture;
    std::vector<gui::Button> _buttons;
};

}  // namespace systems
