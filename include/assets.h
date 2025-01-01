#pragma once

#include "../raylib/raylib.h"

enum OctaveIcon
{
    ICON_BATTERY_MID = 220,
    ICON_MUSIC = 221,
    ICON_ALBUM = 222,
    ICON_ARTIST = 223,

};

void init_texture_map();
void cleanup_texture_map();

// set width/height to -1 if want to keep the original size
Texture2D getTexure(const char *asset_name, int width, int height);
