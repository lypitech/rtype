#include "Menus.hpp"

#include <raylib.h>

#include "app.hpp"
#include "enums/game_state.hpp"
#include "enums/menu_state.hpp"
#include "packets/client/lobby_list.hpp"

namespace gui {

LobbyButton::LobbyButton(const uint32_t& roomId,
                         Rectangle bounds,
                         const std::shared_ptr<Font>& font)
    : _roomId(roomId),
      _font(font),
      _bounds(bounds)
{
}

bool LobbyButton::render() const
{
    const Vector2 mouse = GetMousePosition();

    const bool hover = CheckCollisionPointRec(mouse, _bounds);
    const bool pressed = hover && IsMouseButtonDown(MOUSE_LEFT_BUTTON);

    const std::string msg = std::to_string(_roomId);
    const Vector2 pos = {_bounds.x + _bounds.width / 2 - (80 + 3) / 4 * msg.size(),
                         _bounds.y + _bounds.height / 2 - 80 / 2};
    DrawTextEx(*_font, msg.c_str(), pos, 80, 3, WHITE);
    if (hover) {
        DrawRectangleRounded(_bounds, .18, 25, {255, 255, 255, 255 / 2});
    }
    DrawRectangleRoundedLinesEx(_bounds, .18, 25, 4, WHITE);
    return pressed;
}

}  // namespace gui

namespace systems {

MenuRenderer::MenuRenderer(client::Lobby& lobby,
                           service::Network& service,
                           rteng::GameEngine& engine)
    : ASystem("MenuRenderer"),
      _service(service),
      _lobby(lobby),
      _engine(engine),
      _texture(BACKGROUND_TEXTURE_FILEPATH.data())
{
    _buttons.reserve(4);
    constexpr float buttonScale = 1;
    constexpr float bWidth = 500 * buttonScale;
    constexpr float bHeight = 150 * buttonScale;
    constexpr float x = 1920.0 / 2 - bWidth / 2;
    constexpr float y = 1080.0 / 2 - bHeight / 2;
    _buttons.emplace_back(
        PLAY_TEXTURE_FILEPATH.data(), Rectangle{x, y - bHeight * 1.5, bWidth, bHeight});
    _buttons.emplace_back(START_TEXTURE_FILEPATH.data(), Rectangle{x, y, bWidth, bHeight});
    _buttons.emplace_back(
        CREDITS_TEXTURE_FILEPATH.data(), Rectangle{x, y + bHeight * 1.5, bWidth, bHeight});
    _basicFont = std::make_shared<Font>(LoadFontEx(FONT_FILEPATH.data(), 120, nullptr, 250));
    _dyslexicFont =
        std::make_shared<Font>(LoadFontEx(DYSLEXIC_FONT_FILEPATH.data(), 120, nullptr, 250));
    _currentFont = _basicFont;
}

void MenuRenderer::apply(rtecs::ECS&)
{
    if (_engine.getGameState() == game::state::GameRunning || WindowShouldClose()) {
        return;
    }
    const size_t menuState = _engine.getMenuState();
    BeginDrawing();
    ClearBackground(GREEN);
    DrawTexture(_texture.getTexture(), 0, 0, WHITE);

    if (menuState == menu::state::MenuLobby) {
        if (_buttons[1].render()) {
            _service.send(packet::Start{});
        }
    }

    if (menuState == menu::state::MenuJoin) {
        DrawTextEx(*_currentFont, "Lobby", {BOX_X + 20, 150}, 120, 3, WHITE);
        DrawRectangleRoundedLinesEx({BOX_X, BOX_Y, BOX_WIDTH, BOX_HEIGHT}, .18, 40, 6, WHITE);
        if (_lobbyButtons.empty()) {
            int column = 0;
            int row = 0;
            for (size_t i = 1; i < _lobby.roomIds.size() + 1; ++i) {
                const Rectangle bounds = {
                    BOX_IN_BOX_ORIGIN_X +
                        (BOX_IN_BOX_WIDTH + BETWEEN_BOX_OFFSET_X) * 1.0f * (column),
                    BOX_IN_BOX_ORIGIN_Y + (BOX_IN_BOX_HEIGHT + BETWEEN_BOX_OFFSET_Y) * 1.0f * row,
                    BOX_IN_BOX_WIDTH,
                    BOX_IN_BOX_HEIGHT};
                column++;
                if (i % 5 == 0) {
                    row++;
                    column = 0;
                }
                _lobbyButtons.emplace_back(_lobby.roomIds[i - 1], bounds, _currentFont);
            }
            LOG_TRACE_R1("Created {} lobbies", _lobbyButtons.size());
        }
        for (const auto& lobbyButton : _lobbyButtons) {
            if (lobbyButton.render()) {
                _service.send(packet::Join{"username", lobbyButton.getRoomId()});
            }
        }
    }

    if (menuState == menu::state::MenuHome) {
        // Display the settings button.
        if (_buttons[0].render()) {
            _engine.setMenuState(menu::state::MenuJoin);
            _service.send(packet::LobbyList{});
        }
        if (_buttons[2].render()) {
            LOG_INFO("Project R-type, created at Epitech during 3rd year.");
            // TODO: Display credits
        }
    }
    EndDrawing();
}

}  // namespace systems
