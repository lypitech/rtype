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

struct SpriteConfig
{
    int frameCount;
    int frameWidth;
    int frameHeight;
    float frameTime;
};

struct AnimationConfig
{
    int frameCount;
    float frameTime;
    int frameWidth;
    int frameHeight;
};

static const std::unordered_map<entity::Type, AnimationConfig> entityTypeToAnimation = {
    {entity::Type::kPlayer,
     {5, 0.1f, 33, 15}},  // {Number of frames, Time per frame, Frame width, Frame height}
    {entity::Type::kEnemy, {8, 0.15f, 32, 32}},
    {entity::Type::kBullet, {3, 0.0f, 18, 14}}};

inline AnimationConfig typeToAnimation(const entity::Type& type)
{
    if (!entityTypeToAnimation.contains(type)) {
        return {1, 0.0f, 0, 0};
    }
    return entityTypeToAnimation.at(type);
}

class AssetManager
{
public:
    AssetManager() = default;
    ~AssetManager() = default;

    void init();

    [[nodiscard]] const gui::Texture& getTexture(entity::Type id) const;

    [[nodiscard]] const Texture2D& getBackground() const;

    [[nodiscard]] const gui::Texture& getUITexture(gui::UIAsset id) const;

    [[nodiscard]] const gui::SpriteConfig& getSpriteConfig(entity::Type id) const;

private:
    std::unique_ptr<gui::Texture> _background;
    std::vector<gui::Texture> _textures;
    std::map<entity::Type, gui::SpriteConfig> _spriteConfigs;
    std::map<gui::UIAsset, std::unique_ptr<gui::Texture>> _uiTextures;
};

}  // namespace gui
