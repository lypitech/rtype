#include "Menus.hpp"

#include <raylib.h>

#include "app.hpp"
#include "enums/game_state.hpp"
#include "enums/menu_state.hpp"
#include "packets/client/lobby_list.hpp"

namespace gui {

LobbyButton::LobbyButton(const uint16_t& roomId,
                         const Rectangle bounds,
                         const std::shared_ptr<Font>& font)
    : _roomId(roomId),
      _font(font),
      _fontSize(80),
      _bounds(bounds)
{
    const std::string msg = std::to_string(_roomId);
    _pos = {_bounds.x + _bounds.width / 2 - (_fontSize + SPACING) / 4 * msg.size(),
            _bounds.y + _bounds.height / 2 - _fontSize / 2};
}

bool LobbyButton::render() const
{
    const Vector2 mouse = GetMousePosition();

    const bool hover = CheckCollisionPointRec(mouse, _bounds);
    const bool pressed = hover && IsMouseButtonDown(MOUSE_LEFT_BUTTON);

    const std::string msg = std::to_string(_roomId);

    DrawTextEx(*_font, msg.c_str(), _pos, _fontSize, SPACING, WHITE);
    if (hover) {
        DrawRectangleRounded(_bounds, ROUND, SMALL_RECT_SEG, SEMI_WHITE);
    }
    DrawRectangleRoundedLinesEx(_bounds, ROUND, SMALL_RECT_SEG, 4, WHITE);
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
    _buttons.reserve(5);

    _buttons.emplace_back(PLAY_BUTTON);
    _buttons.emplace_back(START_BUTTON);
    _buttons.emplace_back(CREDITS_BUTTON);
    _buttons.emplace_back(CREATE_BUTTON);
    _basicFont = SHARE_FONT(BASIC_FONT.data());
    _dyslexicFont = SHARE_FONT(DYSLEXIC_FONT.data());
    _currentFont = _basicFont;
}

void MenuRenderer::renderLobbyList()
{
    DrawTextEx(*_currentFont, "Lobby", {BOX_X + 20, 150}, FONT_SIZE, SPACING, WHITE);
    DrawRectangleRoundedLinesEx({BOX_X, BOX_Y, BOX_WIDTH, BOX_HEIGHT}, ROUND, 40, 6, WHITE);
    if (_lobbyButtons.empty()) {
        float column = 0;
        float row = 0;
        for (size_t i = 1; i < _lobby.roomIds.size() + 1; ++i) {
            const Rectangle bounds = {
                BOX_IN_BOX_ORIGIN_X + (BOX_IN_BOX_WIDTH + BETWEEN_BOX_OFFSET_X) * column,
                BOX_IN_BOX_ORIGIN_Y + (BOX_IN_BOX_HEIGHT + BETWEEN_BOX_OFFSET_Y) * row,
                BOX_IN_BOX_WIDTH,
                BOX_IN_BOX_HEIGHT};
            column++;
            if (i % LOBBY_PER_LINE == 0) {
                row++;
                column = 0;
            }
            _lobbyButtons.emplace_back(_lobby.roomIds[i - 1], bounds, _currentFont);
        }
        LOG_TRACE_R1("Created {} lobbies", _lobbyButtons.size());
    }
    if (_buttons[3].render()) {
        _service.send(packet::Join{"username", 256});
    }
    for (const auto& lobbyButton : _lobbyButtons) {
        if (lobbyButton.render()) {
            _service.send(packet::Join{"username", lobbyButton.getRoomId()});
        }
    }
}

void MenuRenderer::renderHomeMenu() const
{
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

void MenuRenderer::renderPreGameMenu()
{
    if (_buttons[1].render()) {
        _service.send(packet::Start{});
    }
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
        renderPreGameMenu();
    }

    if (menuState == menu::state::MenuJoin) {
        renderLobbyList();
    }

    if (menuState == menu::state::MenuHome) {
        renderHomeMenu();
    }

    EndDrawing();
}

}  // namespace systems
