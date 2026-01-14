#include "assetManager.hpp"

#include <string_view>

#include "logger/Logger.h"

static constexpr std::string_view ENEMY_TEXTURE_PATH = "Client/assets/sprites/enemyAerial.gif";
static constexpr std::string_view BULLET_TEXTURE_PATH = "Client/assets/sprites/bullets.gif";

// static constexpr std::string_view UI_MENU_BG_PATH = "../../Client/assets/ui/MenuBackground.png";
// static constexpr std::string_view UI_PAUSE_BTN_PATH = "../../Client/assets/ui/PauseButton.png";

void AssetManager::init()
{
    size_t entityCount = static_cast<size_t>(entity::Type::kBullet) + 1;
    _textures.reserve(entityCount);

    _textures.emplace_back(gui::PLAYER_TEXTURE_FILEPATH.data(), 0.2f);

    _textures.emplace_back(ENEMY_TEXTURE_PATH.data(), 0.15f);

    _textures.emplace_back(BULLET_TEXTURE_PATH.data(), 0.1f);

    // _uiTextures[gui::UIAsset::kMenuBackground] =
    //     std::make_unique<gui::Texture>(UI_MENU_BG_PATH.data(), 1.0f);
    // _uiTextures[gui::UIAsset::kPauseButton] =
    //     std::make_unique<gui::Texture>(UI_PAUSE_BTN_PATH.data(), 0.5f);

    LOG_INFO("AssetManager: Loaded {} game textures and {} UI textures",
             _textures.size(),
             _uiTextures.size());
}

const gui::Texture& AssetManager::getTexture(entity::Type id) const
{
    size_t index = static_cast<size_t>(id);
    if (index >= _textures.size()) {
        LOG_ERR("AssetManager: Attempting to access unknown game texture index {}", index);
        return _textures[0];  // Return player texture as fallback
    }
    return _textures[index];
}

const gui::Texture& AssetManager::getUITexture(gui::UIAsset id) const
{
    auto it = _uiTextures.find(id);
    if (it == _uiTextures.end()) {
        LOG_ERR("AssetManager: UI texture not found");
        throw std::runtime_error("UI Texture missing");
    }
    return *(it->second);
}
