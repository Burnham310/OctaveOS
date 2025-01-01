#pragma once

#include <stdio.h>
#include <stdbool.h>

#define draw_text_at_mid(height, text, fontsize, fontcolor) \
    DrawText(text, GetScreenWidth() / 2 - MeasureText(text, fontsize) / 2, height, fontsize, fontcolor)

#define oct_printf(fmt, ...) fprintf(stderr, "[OctaveOS] " fmt "\n", ##__VA_ARGS__);
#define oct_eprintf(fmt, ...) fprintf(stderr, "[💀 OctaveOS] " fmt "\n", ##__VA_ARGS__)
#define oct_ASSERT(bool, task) \
    do                         \
    {                          \
        if (!(bool))           \
        {                      \
            task;              \
        }                      \
    } while (0)


#define COMPONENT_LOADING_ORDER 7
#define COMPONENT_REGISTER_ORDER 5