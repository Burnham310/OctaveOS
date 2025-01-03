#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>

#include "metalib.h"
// #include "CONFIG.h"
#include "tag_c.h"
#include "utils.h"

#define COVER_THUMBNAIL_DIR "cover_thumbnail"
#define MAX_PATH_LENGTH 512


int process_file(const char *file_path)
{
    TagLib_File *file = taglib_file_new(file_path);
    if (!taglib_file_is_valid(file))
        return 0;

    TagLib_Tag *tag = taglib_file_tag(file);
    TagLib_AudioProperties *properties = taglib_file_audioproperties(file);
    if (!tag || !properties)
    {
        fprintf(stderr, "Error: Failed to read metadata from file %s\n", file_path);
        taglib_file_free(file);
        return -1;
    }

    Track track = {0};

    strncpy(track.track_path, file_path, sizeof(track.track_path) - 1);
    strncpy(track.track_name, taglib_tag_title(tag), sizeof(track.track_name) - 1);
    strncpy(track.artist.artist_name, taglib_tag_artist(tag), sizeof(track.artist.artist_name) - 1);
    strncpy(track.album.album_name, taglib_tag_album(tag), sizeof(track.album.album_name) - 1);
    track.bitrate = taglib_audioproperties_bitrate(properties);
    track.sample_rate = taglib_audioproperties_samplerate(properties);
    track.channels = taglib_audioproperties_channels(properties);
    track.length = taglib_audioproperties_length(properties);

    taglib_file_free(file);

    // Add track metadata to the database
    if (metalib_add_raw_data(&track) != 0)
    {
        fprintf(stderr, "Error: Failed to add metadata for file %s\n", file_path);
        return -1;
    }

    return 0;
}

void process_directory(const char *dir_path)
{
    DIR *dir = opendir(dir_path);
    if (!dir)
    {
        fprintf(stderr, "Error: Cannot open directory %s\n", dir_path);
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        char file_path[MAX_PATH_LENGTH];
        snprintf(file_path, sizeof(file_path), "%s/%s", dir_path, entry->d_name);

        struct stat st;
        if (stat(file_path, &st) == -1)
        {
            continue;
        }

        if (S_ISDIR(st.st_mode))
        {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            {
                continue;
            }
            process_directory(file_path);
        }
        else if (S_ISREG(st.st_mode))
        {
            if (process_file(file_path) != 0)
            {
                fprintf(stderr, "Error: Failed to process file %s\n", file_path);
            }
        }
    }

    closedir(dir);
}

// int main(int argc, char *argv[])
// {
//     if (argc != 2)
//     {
//         fprintf(stderr, "Usage: %s <directory_path>\n", argv[0]);
//         return EXIT_FAILURE;
//     }

//     const char *dir_path = argv[1];

//     ensure_directory_exists(COVER_THUMBNAIL_DIR);

//     if (init_metalib("metalib.db", "metadb.sql") != 0)
//     {
//         fprintf(stderr, "Error: Failed to initialize metadata library\n");
//         return EXIT_FAILURE;
//     }

//     process_directory(dir_path);

//     close_metalib();
//     return EXIT_SUCCESS;
// }
