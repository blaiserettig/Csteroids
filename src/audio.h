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

typedef struct {
    audio_clip fire;
    audio_clip explode;
    audio_clip saucer;
    audio_clip small_asteroid_hit;
    audio_clip medium_asteroid_hit;
    audio_clip big_asteroid_hit;
    audio_clip respawn;
    audio_clip game_over;
    audio_clip new_stage;
    audio_clip booster;
    audio_clip bloop_lo;
    audio_clip bloop_hi;
    audio_clip button_hover;
    audio_clip button_select;
    audio_clip armor_hit;
    audio_clip lucky;
    audio_clip coin;
    audio_clip no;
} audio_clips_struct;

typedef enum {
    AUDIO_STREAM_FIRE,
    AUDIO_STREAM_ASTEROID,
    AUDIO_STREAM_SHIP_FIRE,
    AUDIO_STREAM_SHIP_BOOSTER,
    AUDIO_STREAM_SAUCER,
    AUDIO_STREAM_MUSIC,
    AUDIO_STREAM_SFX,
    AUDIO_STREAM_COUNT
} audio_stream_type;

extern audio_clips_struct audio_clips;

int init_audio(void);
void play_sound_effect(audio_stream_type stream_type, audio_clip clip);
void free_audio_clip(audio_clip *clip);
int load_all_audio(void);

void init_saucer_sound(void);
void play_saucer_sound(void);
void stop_saucer_sound(void);
void cleanup_saucer_sound(void);
void keep_saucer_sound_playing(void);

void music_start(void);
void music_stop(void);
void music_update(void);
void music_cleanup(void);

#endif // AUDIO_H