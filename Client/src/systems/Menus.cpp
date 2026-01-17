#include "Menus.hpp"

#include <raylib.h>

#include "enums/game_state.hpp"
#include "enums/menu_state.hpp"

namespace systems {

MenuRenderer::MenuRenderer(service::Network& service,
                           rteng::GameEngine& engine)
    : ASystem("MenuRenderer"),
      _service(service),
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
        // Display all the joinable rooms as buttons.
        if (false /* ButtonPressed */) {
            _service.send(packet::Join{"username", 0 /*roomID*/});
        }
    }

    if (menuState == menu::state::MenuHome) {
        // Display the settings button.
        if (_buttons[0].render()) {
            _engine.setMenuState(menu::state::MenuJoin);
        }
        if (_buttons[2].render()) {
            LOG_INFO("Project R-type, created at Epitech during 3rd year.");
            // TODO: Display credits
        }
    }
    EndDrawing();
}

}  // namespace systems
