// audio.h
#ifndef AUDIO_H
#define AUDIO_H

#include <SDL3/SDL.h>

typedef struct {
    SDL_AudioSpec wav_spec;
    Uint8* data;
    Uint32 length;
} audio_clip;

typedef struct {
    const char* filename;
    audio_clip* destination;
} audio_file_entry;

char* get_asset_path(const char* filename);
int init_audio(void);
void play_sound_effect(const audio_clip clip);
int load_audio_clip(const char *filename, audio_clip *clip);
void free_audio_clip(audio_clip *clip);
int load_audio_file(const char* filename, audio_clip* clip);
int load_audio_files(const audio_file_entry* files, const int count);
int load_all_audio(void);

#endif // AUDIO_H