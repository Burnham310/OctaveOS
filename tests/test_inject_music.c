#include "testcase.h"
#include <string.h>

static Music music;

void test_inject_music()
{
    oct_printf("start test music injection");

    InitAudioDevice();
    music = LoadMusicStream("tests/sample.wav");

    if (!IsMusicValid(music))
    {
        CloseAudioDevice();
        CloseWindow();
        printf("Failed to load music.\n");
        return;
    }

    PlayMusicStream(music);

    SYSTEM_STATE.music_state = MUSIC_PLAYING;
    SYSTEM_STATE.music_stream = music.stream;
    SYSTEM_STATE.music_task_id = 1;
    strcpy(SYSTEM_STATE.music_name, "Fuwa Fuwa Time");
    strcpy(SYSTEM_STATE.music_artist, "Houkago Tea Time");
}

void test_inject_music_loop()
{
    UpdateMusicStream(music);
}