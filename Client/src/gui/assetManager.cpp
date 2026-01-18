#include "assetManager.hpp"

#include <string_view>

#include "logger/Logger.h"

static constexpr std::string_view ENEMY_TEXTURE_PATH = "Client/assets/sprites/enemyAerial.gif";
static constexpr std::string_view BULLET_TEXTURE_PATH = "Client/assets/sprites/bullet.gif";

// static constexpr std::string_view UI_MENU_BG_PATH = "../../Client/assets/ui/MenuBackground.png";
// static constexpr std::string_view UI_PAUSE_BTN_PATH = "../../Client/assets/ui/PauseButton.png";

void gui::AssetManager::init()
{
    _background = std::make_unique<gui::Texture>(BACKGROUND_TEXTURE_FILEPATH.data());
    size_t entityCount = static_cast<size_t>(entity::Type::kBullet) + 1;
    _textures.reserve(entityCount);

    _textures.emplace_back(gui::PLAYER_TEXTURE_FILEPATH.data(), 1.0f);
    _textures.emplace_back(ENEMY_TEXTURE_PATH.data(), 1.0f);
    _textures.emplace_back(BULLET_TEXTURE_PATH.data(), 1.0f);

    LOG_INFO("AssetManager: Loaded {} game textures and {} UI textures",
             _textures.size(),
             _uiTextures.size());
    // _uiTextures[gui::UIAsset::kMenuBackground] =
    //     std::make_unique<gui::Texture>(UI_MENU_BG_PATH.data(), 1.0f);
    // _uiTextures[gui::UIAsset::kPauseButton] =
    //     std::make_unique<gui::Texture>(UI_PAUSE_BTN_PATH.data(), 0.5f);
}

const gui::Texture& gui::AssetManager::getTexture(entity::Type id) const
{
    size_t index = static_cast<size_t>(id);
    if (index >= _textures.size()) {
        LOG_ERR("AssetManager: Attempting to access unknown game texture index {}", index);
        return _textures[0];
    }
    return _textures[index];
}

const Texture2D& gui::AssetManager::getBackground() const
{
    if (!_background) {
        return _textures.back().getTexture();
    }
    return _background->getTexture();
}

const gui::Texture& gui::AssetManager::getUITexture(gui::UIAsset id) const
{
    auto it = _uiTextures.find(id);
    if (it == _uiTextures.end()) {
        LOG_ERR("AssetManager: UI texture not found");
        throw std::runtime_error("UI Texture missing");
    }
    return *(it->second);
}
