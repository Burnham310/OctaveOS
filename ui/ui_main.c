#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include "state.h"
#include "assets.h"
#include "utils.h"

#include "components.h"

#define MAIN_MENU_BTN(row, col, icon_tag, text)                                                             \
    GuiDrawIcon(icon_tag, col ? GetScreenWidth() - 170 : 70, 70 + 160 * row, 6, SYSTEM_STATE.ui_fontColor); \
    DrawText(text, (col ? GetScreenWidth() - 170 : 70) + 50 - MeasureText(text, 20) / 2,                    \
             180 + 160 * row, 20, SYSTEM_STATE.ui_fontColor);

__attribute__((constructor(COMPONENT_LOADING_ORDER))) static void init()
{
    printf("asd\n");
    load_component("spectrogram");
}

void RENDER_UI_MAIN()
{
    ClearBackground(BLACK);
    DrawText("20/56", 10, 10, 5, SYSTEM_STATE.ui_fontColor);
    GuiDrawIcon(ICON_REREDO_FILL, GetScreenWidth() - RAYGUI_ICON_SIZE - 45, 7, 1, SYSTEM_STATE.ui_fontColor);
    DrawText("55%", GetScreenWidth() - RAYGUI_ICON_SIZE - 26, 10, 5, SYSTEM_STATE.ui_fontColor);
    GuiDrawIcon(ICON_BATTERY_MID, GetScreenWidth() - 20, 7, 1, SYSTEM_STATE.ui_fontColor);

    MAIN_MENU_BTN(0, 0, ICON_FILETYPE_AUDIO, "PLAYLIST");
    MAIN_MENU_BTN(1, 0, ICON_MUSIC, "TRACKS");
    MAIN_MENU_BTN(2, 0, ICON_ARTIST, "ARTIST");

    MAIN_MENU_BTN(0, 1, ICON_ALBUM, "ALBUMS");
    MAIN_MENU_BTN(1, 1, ICON_FOLDER_FILE_OPEN, "FOLDER");
    MAIN_MENU_BTN(2, 1, ICON_GEAR_BIG, "SETTING");

    Vector2 vinyl_center = {GetScreenWidth() / 2.0f, GetScreenHeight() - 220};
    float coverSize = 80.0f;

    DrawCircleGradient(vinyl_center.x, vinyl_center.y, 100.0f, SYSTEM_STATE.ui_dynamicColor, (Color){0, 0, 0, 0});
    Rectangle sourceRec = {0.0f, 0.0f, (float)(SYSTEM_STATE.music_cover.width), (float)(SYSTEM_STATE.music_cover.width)};
    Rectangle destRec = {vinyl_center.x, vinyl_center.y, coverSize, coverSize};
    Vector2 origin = {coverSize / 2.0f, coverSize / 2.0f};

    DrawTexturePro(SYSTEM_STATE.music_cover, sourceRec, destRec, origin, fmodf(SYSTEM_STATE.ui_animation_seed * 50.0f, 360.0f), SYSTEM_STATE.ui_fontColor);

    // Draw the song name (scrolling if needed)
    draw_text_at_mid(vinyl_center.y + 50, SYSTEM_STATE.music_name, 15, WHITE);
    draw_text_at_mid(vinyl_center.y + 70, SYSTEM_STATE.music_artist, 10, GRAY);

    float buttonRadius = 30.0f;
    float buttonSpacing = 80.0f;
    float buttonY = GetScreenHeight() - 80.0f;

    // Render neon bottom decoration
    Texture2D neon_decoration = getTexure("assets/neon_texture.png", GetScreenWidth(), -1);
    DrawTexture(neon_decoration, 0, GetScreenHeight() - neon_decoration.height, WHITE);

    // Render Spectrugram
    render_component("spectrogram", (Rectangle){.x = 0, .y = GetScreenHeight() - 200, .width = GetScreenWidth(), .height = 200});

    // Play Button
    Vector2 playButtonCenter = {GetScreenWidth() / 2, buttonY};
    DrawCircle(playButtonCenter.x, playButtonCenter.y, buttonRadius, RED);
    GuiDrawIcon(ICON_PLAYER_PAUSE, playButtonCenter.x - RAYGUI_ICON_SIZE,
                playButtonCenter.y - RAYGUI_ICON_SIZE, 2, WHITE);

    // Previous Button
    Vector2 prevButtonCenter = {playButtonCenter.x - buttonSpacing, buttonY};
    DrawCircle(prevButtonCenter.x, prevButtonCenter.y, buttonRadius, DARKGRAY);
    GuiDrawIcon(ICON_PLAYER_PLAY_BACK, prevButtonCenter.x - RAYGUI_ICON_SIZE,
                prevButtonCenter.y - RAYGUI_ICON_SIZE, 2, WHITE);

    // Next Button
    Vector2 nextButtonCenter = {playButtonCenter.x + buttonSpacing, buttonY};
    DrawCircle(nextButtonCenter.x, nextButtonCenter.y, buttonRadius, DARKGRAY);
    GuiDrawIcon(ICON_PLAYER_PLAY, nextButtonCenter.x - RAYGUI_ICON_SIZE,
                nextButtonCenter.y - RAYGUI_ICON_SIZE, 2, WHITE);
}
