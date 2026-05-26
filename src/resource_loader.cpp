#define CloseWindow Win32_CloseWindow
#define ShowCursor Win32_ShowCursor
#define Rectangle Win32_Rectangle
#define PlaySound Win32_PlaySound
#define LoadImage Win32_LoadImage
#define DrawText Win32_DrawText
#define DrawTextEx Win32_DrawTextEx

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#undef CloseWindow
#undef ShowCursor
#undef Rectangle
#undef PlaySound
#undef LoadImage
#undef DrawText
#undef DrawTextEx

#include "game.h"

Texture2D LoadTextureFromResource(int id) {
    Texture2D texture = { 0 };
    HRSRC hResInfo = FindResource(NULL, MAKEINTRESOURCE(id), RT_RCDATA);
    if (!hResInfo) return texture;
    HGLOBAL hResData = LoadResource(NULL, hResInfo);
    if (!hResData) return texture;
    int size = SizeofResource(NULL, hResInfo);
    unsigned char* data = (unsigned char*)LockResource(hResData);
    if (!data) return texture;

    Image img = LoadImageFromMemory(".png", data, size);
    texture = LoadTextureFromImage(img);
    UnloadImage(img);
    return texture;
}

Sound LoadSoundFromResource(int id) {
    Sound sound = { 0 };
    HRSRC hResInfo = FindResource(NULL, MAKEINTRESOURCE(id), RT_RCDATA);
    if (!hResInfo) return sound;
    HGLOBAL hResData = LoadResource(NULL, hResInfo);
    if (!hResData) return sound;
    int size = SizeofResource(NULL, hResInfo);
    unsigned char* data = (unsigned char*)LockResource(hResData);
    if (!data) return sound;

    Wave wave = LoadWaveFromMemory(".ogg", data, size);
    sound = LoadSoundFromWave(wave);
    UnloadWave(wave);
    return sound;
}
