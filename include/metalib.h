#pragma once

#include <sqlite3.h>

#define METALIB_DEFAULT_THUMBNAIL_PATH ""
#define METALIB_DEFAULT_ARTIST \
    (Artist) { .artist_id = -1 }

typedef struct
{
    char db_name[256];
    sqlite3 *db;
} Metalib;

typedef struct
{
    int artist_id;
    char artist_name[256];
} Artist;

typedef struct
{
    int album_id;
    char album_name[256];
    char thumbnail_path[256];
} Album;

typedef struct
{
    int track_id;
    char track_path[256];
    char track_name[256];
    Album album;
    Artist artist;
    int bitrate;
    int sample_rate;
    int channels;
    double length;
} Track;

int init_metalib(const char *db_path, const char *sql_path);
void close_metalib();

int metalib_add_raw_data(const Track *track);
