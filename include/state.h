#pragma once

#include "../raylib/raylib.h"

typedef struct
{
    enum
    {
        MUSIC_PLAYING,
        MUSIC_PAUSE,
        MUSIC_NULL
    } music_state;
    int music_task_id; // change when play a new track (include replay)
    char music_name[512];
    char music_artist[512];
    AudioStream music_stream;
    Texture2D music_cover;
    Color ui_fontColor;
    Color ui_bgColor;
    Color ui_dynamicColor;
    float ui_animation_seed;
} SystemState;

extern SystemState SYSTEM_STATE;
