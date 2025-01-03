#include "metalib.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "string.h"

#include "utils.h"

static Metalib metalib;

int init_metalib(const char *db_path, const char *sql_path)
{
    if (access(db_path, F_OK) == 0)
    {
        strncpy(metalib.db_name, db_path, sizeof(metalib.db_name) - 1);
        metalib.db_name[sizeof(metalib.db_name) - 1] = '\0';

        if (sqlite3_open(db_path, &metalib.db) != SQLITE_OK)
        {
            oct_eprintf("Failed to open database: %s\n", sqlite3_errmsg(metalib.db));
            sqlite3_close(metalib.db);
            return -1;
        }
        oct_printf("Open database: %s", db_path);

        return 0;
    }

    if (sqlite3_open(db_path, &metalib.db) != SQLITE_OK)
    {
        oct_eprintf("Failed to open database: %s", sqlite3_errmsg(metalib.db));
        return -1;
    }

    FILE *file = fopen(sql_path, "r");
    if (!file)
    {
        oct_eprintf("Failed to open SQL file: %s", sql_path);
        sqlite3_close(metalib.db);
        return -1;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *sql = malloc(length + 1);
    if (!sql)
    {
        oct_eprintf("Memory allocation failed");
        fclose(file);
        sqlite3_close(metalib.db);
        return -1;
    }

    fread(sql, 1, length, file);
    sql[length] = '\0';
    fclose(file);

    char *err_msg = NULL;
    if (sqlite3_exec(metalib.db, sql, NULL, NULL, &err_msg) != SQLITE_OK)
    {
        oct_eprintf("Failed to execute SQL: %s", err_msg);
        sqlite3_free(err_msg);
        free(sql);
        sqlite3_close(metalib.db);
        return -1;
    }

    free(sql);
    oct_printf("create database: %s", db_path);
    strncpy(metalib.db_name, db_path, sizeof(metalib.db_name) - 1);
    metalib.db_name[sizeof(metalib.db_name) - 1] = '\0';

    return 0;
}

void close_metalib()
{
    if (metalib.db)
    {
        sqlite3_close(metalib.db);
        metalib.db = NULL;
    }
    oct_printf("close database: %s", metalib.db_name);
}

static int find_or_add_artist(const Artist *artist)
{
    sqlite3_stmt *stmt;
    const char *select_query = "SELECT artist_id FROM Artists WHERE artist_name = ?;";
    int artist_id = -1;

    if (sqlite3_prepare_v2(metalib.db, select_query, -1, &stmt, NULL) == SQLITE_OK)
    {
        sqlite3_bind_text(stmt, 1, artist->artist_name, -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_ROW)
        {
            artist_id = sqlite3_column_int(stmt, 0);
        }
        sqlite3_finalize(stmt);
    }
    else
    {
        oct_eprintf("Failed to prepare SELECT statement: %s", sqlite3_errmsg(metalib.db));
        return -1;
    }

    // oct_printf("artist id %d", artist_id);

    // If artist not found, insert a new record
    if (artist_id == -1)
    {
        const char *insert_query = "INSERT INTO Artists (artist_name) VALUES (?);";
        if (sqlite3_prepare_v2(metalib.db, insert_query, -1, &stmt, NULL) == SQLITE_OK)
        {
            sqlite3_bind_text(stmt, 1, artist->artist_name, -1, SQLITE_STATIC);

            if (sqlite3_step(stmt) != SQLITE_DONE)
            {
                oct_eprintf("Failed to add artist: %s", sqlite3_errmsg(metalib.db));
                sqlite3_finalize(stmt);
                return -1;
            }

            artist_id = (int)sqlite3_last_insert_rowid(metalib.db);
            sqlite3_finalize(stmt);
        }
        else
        {
            oct_eprintf("Failed to prepare INSERT statement: %s", sqlite3_errmsg(metalib.db));
            return -1;
        }
    }

    return artist_id;
}

static int find_or_add_album(const Album *album)
{
    char query[512];
    snprintf(query, sizeof(query), "SELECT album_id FROM Albums WHERE album_name = '%s';", album->album_name);

    sqlite3_stmt *stmt;
    int album_id = -1;

    if (sqlite3_prepare_v2(metalib.db, query, -1, &stmt, NULL) == SQLITE_OK)
    {
        if (sqlite3_step(stmt) == SQLITE_ROW)
        {
            album_id = sqlite3_column_int(stmt, 0);
        }
        sqlite3_finalize(stmt);
    }

    if (album_id == -1)
    {
        snprintf(query, sizeof(query), "INSERT INTO Albums (album_name, thumbnail_path) VALUES ('%s', '%s');",
                 album->album_name, album->thumbnail_path[0] != '\0' ? "'%s'" : "NULL");
        char *err_msg = NULL;
        if (sqlite3_exec(metalib.db, query, NULL, NULL, &err_msg) != SQLITE_OK)
        {
            oct_eprintf("Failed to add album: %s", err_msg);
            sqlite3_free(err_msg);
            return -1;
        }
        album_id = (int)sqlite3_last_insert_rowid(metalib.db);
    }

    return album_id;
}

int metalib_add_raw_data(const Track *track)
{
    int artist_id = -1;
    if (track->artist.artist_name[0] != '\0')
    {
        artist_id = find_or_add_artist(&track->artist);
        if (artist_id == -1)
        {
            return -1;
        }
    }

    int album_id = -1;
    if (track->album.album_name[0] != '\0')
    {
        album_id = find_or_add_album(&track->album);
        if (album_id == -1)
        {
            return -1;
        }
    }

    char query[1024];
    char album_id_str[32], artist_id_str[32];

    if (album_id != -1)
        snprintf(album_id_str, sizeof(album_id_str), "%d", album_id);
    else
        strcpy(album_id_str, "NULL");

    if (artist_id != -1)
        snprintf(artist_id_str, sizeof(artist_id_str), "%d", artist_id);
    else
        strcpy(artist_id_str, "NULL");

    snprintf(query, sizeof(query),
             "INSERT INTO Tracks (track_path, track_name, album_id, artist_id, bitrate, sample_rate, channels, length) "
             "VALUES ('%s', '%s', %s, %s, %d, %d, %d, %f);",
             track->track_path, track->track_name, album_id_str, artist_id_str,
             track->bitrate, track->sample_rate, track->channels, track->length);

    char *err_msg = NULL;
    if (sqlite3_exec(metalib.db, query, NULL, NULL, &err_msg) != SQLITE_OK)
    {
        oct_eprintf("Failed to add track: %s", err_msg);
        sqlite3_free(err_msg);
        return -1;
    }
    else
    {
        oct_printf("add track: %s", track->track_name);
    }

    return 0;
}
