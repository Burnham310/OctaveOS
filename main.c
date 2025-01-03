#include "raylib.h"
#include "raygui.h"
#include "math.h"

#include "assert.h"
#include "assets.h"
#include "state.h"
#include "metalib.h"
#include "components.h"

#include "ui_main.h"

#define WINDOW_WIDTH 480
#define WINDOW_HEIGHT 800
#define WINDOW_SIZE WINDOW_WIDTH, WINDOW_HEIGHT

#ifdef TEST
#warning TestMode On
#include "tests/testcase.h"
#endif

#define METADB_PATH "metalib.db"
#define METADB_SQL "metadb.sql"

SystemState SYSTEM_STATE;

// DO NOT change priority
__attribute__((constructor(0))) static void oct_init()
{
    oct_ASSERT(!init_metalib(METADB_PATH, METADB_SQL), exit(1));
    init_component_system();
    init_texture_map();
}

__attribute__((destructor)) static void oct_exit()
{
    close_metalib();
    cleanup_component_system();
    cleanup_texture_map();
}

int main()
{
    SetConfigFlags(FLAG_WINDOW_UNDECORATED);
    InitWindow(WINDOW_SIZE, "OctaveOS");
    SetTargetFPS(120);

    SYSTEM_STATE = (SystemState){
        .music_state = MUSIC_NULL,
        .music_task_id = -1,
        .music_cover = getTexure("assets/vinyl.png", -1, -1),
        .ui_animation_seed = 0,
        .ui_bgColor = BLACK,
        .ui_fontColor = WHITE,
    };

    GuiLoadIcons("assets/octave_icons.rgi", false);

#ifdef TEST
    test_inject_music();
#endif

    while (!WindowShouldClose())
    {
        DrawFPS(1, 1);
        // Update Animation Seed
        SYSTEM_STATE.ui_animation_seed += GetFrameTime();
        SYSTEM_STATE.ui_dynamicColor = (Color){
            (unsigned char)(128 + 127 * sinf(SYSTEM_STATE.ui_animation_seed * 1.2f)),
            (unsigned char)(128 + 127 * sinf(SYSTEM_STATE.ui_animation_seed * 1.5f)),
            (unsigned char)(128 + 127 * sinf(SYSTEM_STATE.ui_animation_seed * 1.8f)),
            (unsigned char)(150 + 100 * sinf(SYSTEM_STATE.ui_animation_seed)),
        },

#ifdef TEST
        test_inject_music_loop();
#endif

        BeginDrawing();
        RENDER_UI_MAIN();
        EndDrawing();
    }
}
