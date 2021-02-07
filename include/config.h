// config.h

#ifndef _CONFIG_H
#define _CONFIG_H

#define MASTER_VOLUME 1.0f
#define MUSIC_VOLUME  0.8f
#define SFX_VOLUME    1.0f
#define UI_VOLUME     0.6f

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 960
#define WINDOW_FULLSCREEN 0
#define VSYNC 1

#define WORLD_STORAGE_FILE "resource/map/world.data"
#define WORLD_STORAGE_FILE_BACKUP "resource/map/world.data.backup"

#define LOG_FILE "profile/debug.log"

#define COLOR_WARN      V3(0.80f, 0.23f, 0.32f)
#define COLOR_OK        V3(0.25f, 0.30f, 0.75f)
#define COLOR_ACCEPT    V3(0.30f, 0.70f, 0.20f)
#define COLOR_MESSAGE   V3(0.85f, 0.79f, 0.20f)


// TODO(lucas): Move these to another file later (probably a language type of file, in case we want to support multiple languages)!

#define M_SILVER_KEY_DESTROYED "A silver key got destroyed!"
#define M_DIED "You died..."

#endif
