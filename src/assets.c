#include "assets.h"
#include "stdio.h"

#include "stb_ds.h"

static struct
{
    const char *key;
    Texture2D value;
} *texture_map = NULL;

void init_texture_map()
{
    sh_new_arena(texture_map);
}

void cleanup_texture_map()
{
    // for (size_t i = 0; i < shlen(texture_map); i++)
    // {
    //     UnloadTexture(texture_map[i].value);
    // }
    shfree(texture_map);
}

Texture2D getTexure(const char *asset_name, int width, int height)
{
    int index = shgeti(texture_map, asset_name);
    if (index != -1)
    {
        return texture_map[index].value;
    }

    Image textureImg = LoadImage(asset_name);
    ImageResize(&textureImg, width == -1 ? textureImg.width : width, height == -1 ? textureImg.height : height);
    Texture2D texture = LoadTextureFromImage(textureImg);
    UnloadImage(textureImg);

    shput(texture_map, asset_name, texture);

    return texture;
}
