#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "enums/entity_types.hpp"
#include "gui/texture.hpp"

namespace gui {

enum class UIAsset
{
    kMenuBackground,
    kPauseButton,
    kHealthBar,
    kScoreFont
};

}  // namespace gui

class AssetManager
{
public:
    AssetManager() = default;
    ~AssetManager() = default;

    void init();

    [[nodiscard]] const gui::Texture& getTexture(entity::Type id) const;

    [[nodiscard]] const gui::Texture& getUITexture(gui::UIAsset id) const;

private:
    std::vector<gui::Texture> _textures;
    std::map<gui::UIAsset, std::unique_ptr<gui::Texture>> _uiTextures;
};
