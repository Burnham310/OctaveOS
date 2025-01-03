CREATE TABLE Artists (
    artist_id INTEGER PRIMARY KEY AUTOINCREMENT,
    artist_name TEXT NOT NULL
);

CREATE TABLE Albums (
    album_id INTEGER PRIMARY KEY AUTOINCREMENT,
    album_name TEXT NOT NULL,
    thumbnail_path TEXT NULL
);

CREATE TABLE Tracks (
    track_id INTEGER PRIMARY KEY AUTOINCREMENT,
    track_path TEXT NOT NULL,
    track_name TEXT NOT NULL,
    album_id INTEGER NULL,
    artist_id INTEGER NULL,
    bitrate INTEGER NOT NULL,
    sample_rate INTEGER NOT NULL,
    channels INTEGER NOT NULL,
    length REAL NOT NULL,
     
    FOREIGN KEY (album_id) REFERENCES Albums(album_id) ON DELETE CASCADE,
    FOREIGN KEY (artist_id) REFERENCES Artists(artist_id) ON DELETE CASCADE
);

-- Enable foreign key constraints
PRAGMA foreign_keys = ON;

-- Trigger to auto-delete albums with no tracks
CREATE TRIGGER delete_empty_albums
AFTER DELETE ON Tracks
BEGIN
    DELETE FROM Albums
    WHERE album_id = OLD.album_id
    AND NOT EXISTS (
        SELECT 1 FROM Tracks WHERE album_id = OLD.album_id
    );
END;

-- Trigger to auto-delete artists with no tracks
CREATE TRIGGER delete_empty_artists
AFTER DELETE ON Tracks
BEGIN
    DELETE FROM Artists
    WHERE artist_id = OLD.artist_id
    AND NOT EXISTS (
        SELECT 1 FROM Tracks WHERE artist_id = OLD.artist_id
    );
END;
