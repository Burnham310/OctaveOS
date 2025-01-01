// #include "raylib.h"

// #define RAYGUI_IMPLEMENTATION

// #include "raygui.h"

// #define WINDOW_WIDTH 480
// #define WINDOW_HEIGHT 800
// #define WINDOW_SIZE WINDOW_WIDTH, WINDOW_HEIGHT

// typedef struct Album
// {
//     Texture2D cover;
//     const char *name;
//     int trackCount;
// } Album;

// int main()
// {
//     SetConfigFlags(FLAG_WINDOW_UNDECORATED);
//     InitWindow(WINDOW_SIZE, "OctaveOS - Album Selection");
//     SetTargetFPS(60);

//     Album albums[9] = {
//         {LoadTexture("assets/album_cover.png"), "Album 1", 10},
//         {LoadTexture("assets/album_cover.png"), "Album 2", 8},
//         {LoadTexture("assets/album_cover.png"), "Album 3", 12},
//         {LoadTexture("assets/album_cover.png"), "Album 4", 7},
//         {LoadTexture("assets/album_cover.png"), "Album 1", 10},
//         {LoadTexture("assets/album_cover.png"), "Album 2", 8},
//         {LoadTexture("assets/album_cover.png"), "Album 3", 12},
//         {LoadTexture("assets/album_cover.png"), "Album 4", 7},
//         {LoadTexture("assets/album_cover.png"), "Album 5", 15}};
//     int albumCount = 9;

//     Vector2 scroll = {0, 0};

//     // Main loop
//     while (!WindowShouldClose())
//     {
//         BeginDrawing();
//         ClearBackground(BLACK);

//         DrawRectangle(0, 0, WINDOW_WIDTH, 50, LIGHTGRAY);
//         GuiDrawIcon(ICON_ARROW_LEFT, 10, 15, 1, DARKGRAY); // Left arrow
//         DrawText("Album", WINDOW_WIDTH / 2 - MeasureText("Album", 20) / 2, 15, 20, DARKGRAY);
//         GuiDrawIcon(ICON_BOX_MULTISIZE, WINDOW_WIDTH - 30, 15, 1, DARKGRAY); // Right icon

//         DrawRectangle(0, 50, WINDOW_WIDTH, 40, GRAY);
//         DrawText("Play All", WINDOW_WIDTH / 2 - MeasureText("Play All", 12) / 2, 64, 12, BLACK);
//         GuiDrawIcon(ICON_PLAYER_PLAY, WINDOW_WIDTH / 2 + MeasureText("Play All", 12) / 2 + 2, 62, 1, BLACK);

//         char albumCountText[32];
//         snprintf(albumCountText, sizeof(albumCountText), "%d albums", albumCount);
//         DrawText(albumCountText, WINDOW_WIDTH / 2 - MeasureText(albumCountText, 15) / 2, 100, 15, GRAY);

//         Rectangle view = {0, 130, WINDOW_WIDTH, WINDOW_HEIGHT - 130};
//         Rectangle content = {0, 130, WINDOW_WIDTH - 20, 80 * albumCount};

//         BeginScissorMode(view.x, view.y, view.width, view.height); // Scissor to show scrollable area
//         for (int i = 0; i < albumCount; i++)
//         {
//             float listItemHeight = 80.0f;
//             float y = 130 + i * listItemHeight + scroll.y;

//             // Ensure the album entry is within visible bounds
//             if (y + listItemHeight < 130 || y > WINDOW_HEIGHT)
//                 continue;

//             // Draw album cover with fixed size
//             float albumCoverSize = 50.0f;
//             float albumCoverY = y + (listItemHeight - albumCoverSize) / 2;
//             DrawTexturePro(
//                 albums[i].cover,
//                 (Rectangle){0, 0, (float)albums[i].cover.width, (float)albums[i].cover.height},
//                 (Rectangle){10, albumCoverY, albumCoverSize, albumCoverSize},
//                 (Vector2){0, 0},
//                 0.0f,
//                 WHITE);

//             float textX = 80;
//             float textY = albumCoverY + 5;
//             DrawText(albums[i].name, textX, textY, 20, DARKGRAY);

//             char trackText[32];
//             snprintf(trackText, sizeof(trackText), "%d tracks", albums[i].trackCount);
//             DrawText(trackText, textX, textY + 25, 15, GRAY);

//             if (i != albumCount - 1)
//                 DrawLine(10, y + listItemHeight, WINDOW_WIDTH - 10, y + listItemHeight, LIGHTGRAY);
//         }
//         EndScissorMode();

//         if (content.height > view.height) // Enable scrolling only if content exceeds view height
//         {
//             scroll.y += GetMouseWheelMove() * 30;

//             if (scroll.y > 0)
//                 scroll.y = 0;
//             if (scroll.y < -(content.height - view.height))
//                 scroll.y = -(content.height - view.height);
//         }
//         else
//         {
//             scroll.y = 0; // Reset scroll for small content
//         }

//         if (content.height > view.height) // Show scrollbar only if needed
//         {
//             float scrollbarHeight = (view.height / content.height) * view.height; // Proportional height
//             float scrollbarY = 130 + (-scroll.y / content.height) * view.height;  // Proportional position
//             float scrollbarX = WINDOW_WIDTH - 20;                                 // Position on the right

//             Rectangle scrollbar = {scrollbarX, scrollbarY, 10, scrollbarHeight};

//             // Check for dragging
//             static bool dragging = false;
//             static float dragOffset = 0;

//             if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), scrollbar))
//             {
//                 dragging = true;
//                 dragOffset = GetMousePosition().y - scrollbar.y; // Offset within scrollbar
//             }
//             else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
//             {
//                 dragging = false;
//             }

//             if (dragging)
//             {
//                 // Adjust scrollbar position based on mouse drag
//                 scrollbarY = GetMousePosition().y - dragOffset;

//                 // Clamp scrollbar within the view area
//                 if (scrollbarY < 130)
//                     scrollbarY = 130;
//                 if (scrollbarY + scrollbarHeight > 130 + view.height)
//                     scrollbarY = 130 + view.height - scrollbarHeight;

//                 // Update scroll offset based on scrollbar position
//                 scroll.y = -(scrollbarY - 130) / view.height * content.height;
//             }

//             // Draw the scrollbar
//             DrawRectangleRec(scrollbar, DARKGRAY);     // Background of the scrollbar
//             DrawRectangleLinesEx(scrollbar, 2, BLACK); // Border for visual clarity
//         }

//         EndDrawing();
//     }

//     // Unload resources
//     for (int i = 0; i < albumCount; i++)
//     {
//         UnloadTexture(albums[i].cover);
//     }
//     CloseWindow();

//     return 0;
// }
