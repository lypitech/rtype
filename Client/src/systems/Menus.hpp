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

#define SHARE_FONT(s) std::make_shared<Font>(LoadFontEx(s, FONT_SIZE, nullptr, 250));

static constexpr std::string_view START_BUTTON_FILEPATH = "../../Client/assets/buttons/start.png";
static constexpr std::string_view PLAY_BUTTON_FILEPATH = "../../Client/assets/buttons/play.png";
static constexpr std::string_view CREDITS_BUTTON_FILEPATH =
    "../../Client/assets/buttons/credits.png";
static constexpr std::string_view BACKGROUND_TEXTURE_FILEPATH =
    "../../Client/assets/background.png";
static constexpr std::string_view BASIC_FONT = "../../Client/assets/basic.ttf";
static constexpr std::string_view DYSLEXIC_FONT = "../../Client/assets/dyslexic.ttf";

static constexpr Color SEMI_WHITE = {255, 255, 255, 255 / 2};

static constexpr float ROUND = .18;
static constexpr float BIG_RECT_SEG = 25;
static constexpr float SMALL_RECT_SEG = 16;

static constexpr float BUTTON_SCALE = 1;
static constexpr float BUTTON_WIDTH = 500 * BUTTON_SCALE;
static constexpr float BUTTON_HEIGHT = 150 * BUTTON_SCALE;
static constexpr float X = 1920.0 / 2 - BUTTON_WIDTH / 2;
static constexpr float Y = 1080.0 / 2 - BUTTON_HEIGHT / 2;
static constexpr float BUTTON_OFFSET_Y = BUTTON_HEIGHT * 1.5;

static constexpr int FONT_SIZE = 120;
static constexpr int SPACING = 3;

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

static constexpr int LOBBY_PER_LINE = 5;

#define PLAY_BUTTON_POS \
    Rectangle { X, Y - BUTTON_OFFSET_Y, BUTTON_WIDTH, BUTTON_HEIGHT }
#define PLAY_BUTTON PLAY_BUTTON_FILEPATH.data(), PLAY_BUTTON_POS

#define START_BUTTON \
    START_BUTTON_FILEPATH.data(), Rectangle { X, Y, BUTTON_WIDTH, BUTTON_HEIGHT }

#define CREDITS_POS \
    Rectangle { X, Y + BUTTON_HEIGHT * 1.5, BUTTON_WIDTH, BUTTON_HEIGHT }
#define CREDITS_BUTTON CREDITS_BUTTON_FILEPATH.data(), CREDITS_POS

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
    Vector2 _pos;
    std::shared_ptr<Font> _font;
    int _fontSize;
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

    void renderLobbyList();
    void renderHomeMenu() const;
    void renderPreGameMenu();

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
