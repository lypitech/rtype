#include <gtest/gtest.h>

#include "../../include/Renderer.h"

// Dans votre setup de test GTest
class RaylibRendererTest : public ::testing::Test
{
protected:
    rteng::graphics::RaylibRenderer *renderer = nullptr;

    void SetUp() override { renderer = new rteng::graphics::RaylibRenderer; }

    void TearDown() override
    {
        delete renderer;
        renderer = nullptr;
    }
};

TEST_F(RaylibRendererTest, TextureLoadingReturnsUniqueId)
{
    // Change test_asset.png to the path of your texture
    int id1 = renderer->loadTexture("test_asset.png");
    int id2 = renderer->loadTexture("test_asset.png");

    ASSERT_GT(id1, 0);
    ASSERT_NE(id1, id2);

    ASSERT_TRUE(renderer->m_textures.count(id1));

    renderer->unloadTexture(id1);
    renderer->unloadTexture(id2);
}

// TEST_F(RaylibRendererTest, TextureUnloadingRemovesInternalStorage) { ... }
