// audio.c
#include "audio.h"
#include "main.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

audio_clips_struct audio_clips = {0};

char* get_asset_path(const char* filename) {
    const char* possible_paths[] = {
        "./assets/",
        "../assets/",
        "assets/",
        "./",
    };

    const int num_paths = sizeof(possible_paths) / sizeof(possible_paths[0]);

    for (int i = 0; i < num_paths; i++) {
        const size_t path_len = strlen(possible_paths[i]) + strlen(filename) + 1;
        char* full_path = malloc(path_len);
        if (!full_path) continue;

        snprintf(full_path, path_len, "%s%s", possible_paths[i], filename);

        SDL_IOStream *file = SDL_IOFromFile(full_path, "rb");
        if (file) {
            const Sint64 file_size = SDL_GetIOSize(file);
            SDL_Log("Found file at: %s (size: %lld bytes)", full_path, (long long)file_size);
            SDL_CloseIO(file);
            return full_path;
        }

        SDL_Log("Tried path: %s - not found", full_path);
        free(full_path);
    }

    SDL_Log("Could not find %s in any of the standard locations", filename);
    return NULL;
}


static SDL_AudioStream* create_and_bind_stream(const char* name, const SDL_AudioSpec* spec) {
    SDL_AudioStream* stream = SDL_CreateAudioStream(spec, spec);
    if (!stream) {
        SDL_Log("Failed to create %s audio stream: %s", name, SDL_GetError());
        return NULL;
    }
    SDL_Log("%s audio stream created successfully", name);

    int test_queued = SDL_GetAudioStreamQueued(stream);
    SDL_Log("%s stream test - queued bytes: %d", name, test_queued);
    if (test_queued < 0) {
        SDL_Log("%s stream is invalid immediately after creation!", name);
        SDL_DestroyAudioStream(stream);
        return NULL;
    }

    if (!SDL_BindAudioStream(state.audio_device, stream)) {
        SDL_Log("Failed to bind %s audio stream: %s", name, SDL_GetError());
        SDL_DestroyAudioStream(stream);
        return NULL;
    }
    SDL_Log("%s audio stream bound to device", name);

    test_queued = SDL_GetAudioStreamQueued(stream);
    SDL_Log("%s stream test after binding - queued bytes: %d", name, test_queued);
    if (test_queued < 0) {
        SDL_Log("%s stream became invalid after binding!", name);
        SDL_DestroyAudioStream(stream);
        return NULL;
    }

    return stream;
}

int init_audio(void) {
    SDL_Log("Initializing audio...");

    state.audio_device = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);
    if (!state.audio_device) {
        SDL_Log("SDL_OpenAudioDevice Error: %s", SDL_GetError());
        return 1;
    }
    SDL_Log("Audio device opened successfully: %u", state.audio_device);

    if (state.audio_device == 0) {
        SDL_Log("Invalid audio device ID returned");
        return 1;
    }

    SDL_AudioSpec spec = {0};
    spec.format = SDL_AUDIO_S16;
    spec.channels = 2;
    spec.freq = 44100;

    typedef struct {
        SDL_AudioStream** stream_ptr;
        const char* name;
    } StreamInfo;

    const StreamInfo streams[] = {
        {&state.fire_stream, "fire"},
        {&state.asteroid_stream, "asteroid"},
        {&state.ship_stream, "ship"},
        {&state.saucer_stream, "saucer"}
    };

    const int num_streams = sizeof(streams) / sizeof(streams[0]);

    for (int i = 0; i < num_streams; i++) {
        *streams[i].stream_ptr = create_and_bind_stream(streams[i].name, &spec);
        if (!*streams[i].stream_ptr) {
            SDL_Log("Failed to initialize %s stream", streams[i].name);
            for (int j = 0; j < i; j++) {
                if (*streams[j].stream_ptr) {
                    SDL_DestroyAudioStream(*streams[j].stream_ptr);
                    *streams[j].stream_ptr = NULL;
                }
            }
            SDL_CloseAudioDevice(state.audio_device);
            return 1;
        }
    }

    if (!SDL_ResumeAudioDevice(state.audio_device)) {
        SDL_Log("Failed to resume audio device: %s", SDL_GetError());
        return 1;
    }
    SDL_Log("Audio device resumed");

    SDL_Log("All audio streams initialized successfully");
    return 0;
}

void play_sound_effect(const audio_stream_type stream_type, const audio_clip clip) {
    SDL_Log("Playing sound effect: %u bytes", clip.length);

    SDL_AudioStream* stream = NULL;
    const char* stream_name = NULL;

    switch (stream_type) {
        case AUDIO_STREAM_FIRE:
            stream = state.fire_stream;
            stream_name = "fire";
            break;
        case AUDIO_STREAM_ASTEROID:
            stream = state.asteroid_stream;
            stream_name = "asteroid";
            break;
        case AUDIO_STREAM_SHIP:
            stream = state.ship_stream;
            stream_name = "ship";
            break;
        case AUDIO_STREAM_SAUCER:
            stream = state.saucer_stream;
            stream_name = "saucer";
            break;
        default:
            SDL_Log("Invalid audio stream type: %d", stream_type);
            return;
    }

    if (!stream) {
        SDL_Log("Audio stream %s is not initialized", stream_name);
        return;
    }

    const bool result = SDL_PutAudioStreamData(stream, clip.data, (int) clip.length);
    if (!result) {
        SDL_Log("Failed to put audio data to %s stream: %s", stream_name, SDL_GetError());
    }
}


int load_audio_clip(const char *filename, audio_clip *clip) {
    SDL_Log("Loading audio file: %s", filename);

    clip->data = NULL;
    clip->length = 0;
    memset(&clip->wav_spec, 0, sizeof(clip->wav_spec));

    SDL_AudioSpec wav_spec;
    Uint8 *wav_buffer;
    Uint32 wav_length;

    if (!SDL_LoadWAV(filename, &wav_spec, &wav_buffer, &wav_length)) {
        SDL_Log("Failed to load audio file '%s': %s", filename, SDL_GetError());
        return -1;
    }

    SDL_Log("Loaded WAV: %d Hz, %d channels, format %d, length %u bytes",
            wav_spec.freq, wav_spec.channels, wav_spec.format, wav_length);

    const SDL_AudioSpec target_spec = {.format = SDL_AUDIO_S16, .channels = 2, .freq = 44100};

    if (wav_spec.format == target_spec.format &&
        wav_spec.channels == target_spec.channels &&
        wav_spec.freq == target_spec.freq) {

        SDL_Log("No conversion needed, using original data");
        clip->wav_spec = wav_spec;
        clip->length = wav_length;

        clip->data = malloc(wav_length);
        if (!clip->data) {
            SDL_Log("Failed to allocate memory for audio data");
            SDL_free(wav_buffer);
            return -1;
        }

        SDL_memcpy(clip->data, wav_buffer, wav_length);
        SDL_free(wav_buffer);
        return 0;
    }

    Uint8 *cvt_buffer = NULL;
    int cvt_len = 0;

    SDL_Log("Converting audio format...");
    const bool success = SDL_ConvertAudioSamples(&wav_spec, wav_buffer, (int)wav_length,
                                          &target_spec, &cvt_buffer, &cvt_len);

    SDL_free(wav_buffer);

    if (!success || !cvt_buffer || cvt_len <= 0) {
        SDL_Log("Failed to convert audio: %s", SDL_GetError());
        if (cvt_buffer) {
            SDL_free(cvt_buffer);
        }
        return -1;
    }

    SDL_Log("Converted audio: %d bytes -> %d bytes", wav_length, cvt_len);

    clip->wav_spec = target_spec;
    clip->length = cvt_len;

    clip->data = malloc(cvt_len);
    if (!clip->data) {
        SDL_Log("Failed to allocate memory for converted audio data");
        SDL_free(cvt_buffer);
        return -1;
    }

    SDL_memcpy(clip->data, cvt_buffer, cvt_len);

    SDL_free(cvt_buffer);

    return 0;
}

void free_audio_clip(audio_clip *clip) {
    if (clip && clip->data) {
        free(clip->data);
        clip->data = NULL;
        clip->length = 0;
        memset(&clip->wav_spec, 0, sizeof(clip->wav_spec));
    }
}

int load_audio_file(const char* filename, audio_clip* clip) {
    SDL_Log("Attempting audio file: %s", filename);

    char* file_path = get_asset_path(filename);
    if (!file_path) {
        SDL_Log("Failed to get asset path for %s", filename);
        return -1;
    }

    const int result = load_audio_clip(file_path, clip);
    if (result < 0) {
        SDL_Log("Failed to load audio clip: %s", filename);
        free(file_path);
        return -1;
    }

    SDL_Log("Successfully loaded %s: %u bytes", filename, clip->length);
    free(file_path);
    return 0;
}

int load_audio_files(const audio_file_entry* files, const int count) {
    SDL_Log("Loading %d audio files...", count);

    int loaded = 0;
    for (int i = 0; i < count; i++) {
        if (load_audio_file(files[i].filename, files[i].destination) == 0) {
            loaded++;
        } else {
            SDL_Log("Failed to load %s", files[i].filename);
        }
    }

    SDL_Log("Loaded %d out of %d audio files", loaded, count);
    return (loaded == count) ? 0 : -1;
}

int load_all_audio(void) {
    const audio_file_entry audio_files[] = {
        {"FIRE.wav", &audio_clips.fire},
        {"ASTEROID_HIT.wav", &audio_clips.asteroid_hit},
        {"EXPLODE.wav", &audio_clips.explode},
        {"RESPAWN.wav", &audio_clips.respawn},
        {"GAME_OVER.wav", &audio_clips.game_over},
        {"NEW_STAGE.wav", &audio_clips.new_stage},
    };

    const int file_count = sizeof(audio_files) / sizeof(audio_files[0]);
    return load_audio_files(audio_files, file_count);
}