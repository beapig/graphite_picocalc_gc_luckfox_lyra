// Host sound backend: standard ALSA PCM output via runtime dlopen.
//
// The firmware plays square-wave beeps through the vendored pwm_sound
// driver (GP26/27 PWM). Here the same effects go out through the normal
// Linux audio stack: device "default" or $PICOCALC_ALSA_DEVICE, 48 kHz
// mono S16_LE. libasound.so.2 is resolved at runtime only — zero
// link-time dependencies, the same pattern SDL's X11 backend uses for
// libX11 (SDL itself was built without audio).

#include "platform/sound.hpp"

#include <dlfcn.h>
#include <pthread.h>
#include <unistd.h>

#include <cstdint>
#include <cstdio>
#include <cstdlib>

namespace platform {

namespace {

// Enum/constant values from <alsa/asoundlib.h> (stable ABI):
constexpr int kStreamPlayback = 0;     // SND_PCM_STREAM_PLAYBACK
constexpr int kFormatS16Le = 2;        // SND_PCM_FORMAT_S16_LE
constexpr int kAccessRwInterleaved = 3;  // SND_PCM_ACCESS_RW_INTERLEAVED
constexpr int kErrPipe = -32;          // -EPIPE (underrun)
constexpr unsigned kRate = 48000;
constexpr unsigned kLatencyUs = 40000;  // 40 ms
constexpr int kChunkFrames = 480;       // 10 ms per writei
constexpr int kAmp = 8192;              // ~25% of full scale

// libasound entry points, dlsym'ed once in init().
struct AlsaApi {
    void* lib = nullptr;
    int (*pcm_open)(void** pcm, const char* name, int stream, int mode) = nullptr;
    int (*pcm_close)(void* pcm) = nullptr;
    int (*pcm_set_params)(void* pcm, int format, int access, unsigned channels, unsigned rate,
                          int soft_resample, unsigned latency) = nullptr;
    long (*pcm_writei)(void* pcm, const void* buf, unsigned long frames) = nullptr;
    int (*pcm_recover)(void* pcm, int err, int silent) = nullptr;
};

AlsaApi g_api;
void* g_pcm = nullptr;

// Pending tones (freq, remaining frames). Depth 4: enough for a burst of
// key-repeat error beeps; new tones are dropped rather than queued.
constexpr int kQueueDepth = 4;
struct Tone {
    uint32_t freq;
    uint32_t frames;
};
Tone g_queue[kQueueDepth];
int g_queue_len = 0;
Tone g_current{0, 0};
uint32_t g_phase = 0;  // 32-bit phase accumulator, wraps naturally
pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;

uint32_t phase_step(uint32_t freq) {
    // 2^32 * freq / rate, as a fixed-point step per sample.
    return static_cast<uint32_t>((static_cast<uint64_t>(freq) << 32) / kRate);
}

void* sound_thread(void* /*arg*/) {
    int16_t buf[kChunkFrames];
    for (;;) {
        pthread_mutex_lock(&g_mutex);
        if (g_current.frames == 0) {
            if (g_queue_len > 0) {
                g_current = g_queue[0];
                for (int i = 1; i < g_queue_len; ++i) {
                    g_queue[i - 1] = g_queue[i];
                }
                --g_queue_len;
                g_phase = 0;
            } else {
                pthread_mutex_unlock(&g_mutex);
                usleep(20000);  // Idle poll: 20 ms wakeup latency is fine for beeps
                continue;
            }
        }
        const uint32_t step = phase_step(g_current.freq);
        const int n = g_current.frames < static_cast<uint32_t>(kChunkFrames)
                          ? static_cast<int>(g_current.frames)
                          : kChunkFrames;
        for (int i = 0; i < n; ++i) {
            buf[i] = static_cast<int16_t>((g_phase & 0x80000000u) != 0 ? -kAmp : kAmp);
            g_phase += step;
        }
        g_current.frames -= static_cast<uint32_t>(n);
        pthread_mutex_unlock(&g_mutex);

        long written = g_api.pcm_writei(g_pcm, buf, static_cast<unsigned long>(n));
        if (written < 0) {
            if (written == kErrPipe && g_api.pcm_recover(g_pcm, static_cast<int>(written), 1) >= 0) {
                continue;  // Underrun handled; retry the chunk next loop
            }
            // Device is gone (unplugged / busy) — drop the tone and idle.
            pthread_mutex_lock(&g_mutex);
            g_current.frames = 0;
            g_queue_len = 0;
            pthread_mutex_unlock(&g_mutex);
        }
    }
    return nullptr;
}

bool enqueue(uint32_t freq, uint32_t frames) {
    if (g_pcm == nullptr || freq == 0 || frames == 0) {
        return false;
    }
    pthread_mutex_lock(&g_mutex);
    const bool ok = g_queue_len < kQueueDepth;
    if (ok) {
        g_queue[g_queue_len].freq = freq;
        g_queue[g_queue_len].frames = frames;
        ++g_queue_len;
    }
    pthread_mutex_unlock(&g_mutex);
    return ok;
}

}  // namespace

bool Sound::init() {
    const char* device = std::getenv("PICOCALC_ALSA_DEVICE");
    if (device == nullptr || device[0] == 0) {
        device = "default";
    }

    g_api.lib = ::dlopen("libasound.so.2", RTLD_NOW);
    if (g_api.lib == nullptr) {
        std::printf("alsa: unavailable (no libasound.so.2)\n");
        return false;
    }
    g_api.pcm_open = reinterpret_cast<decltype(g_api.pcm_open)>(
        ::dlsym(g_api.lib, "snd_pcm_open"));
    g_api.pcm_close =
        reinterpret_cast<decltype(g_api.pcm_close)>(::dlsym(g_api.lib, "snd_pcm_close"));
    g_api.pcm_set_params = reinterpret_cast<decltype(g_api.pcm_set_params)>(
        ::dlsym(g_api.lib, "snd_pcm_set_params"));
    g_api.pcm_writei = reinterpret_cast<decltype(g_api.pcm_writei)>(
        ::dlsym(g_api.lib, "snd_pcm_writei"));
    g_api.pcm_recover = reinterpret_cast<decltype(g_api.pcm_recover)>(
        ::dlsym(g_api.lib, "snd_pcm_recover"));
    if (g_api.pcm_open == nullptr || g_api.pcm_close == nullptr || g_api.pcm_set_params == nullptr ||
        g_api.pcm_writei == nullptr || g_api.pcm_recover == nullptr) {
        std::printf("alsa: unavailable (libasound symbols missing)\n");
        return false;
    }

    if (g_api.pcm_open(&g_pcm, device, kStreamPlayback, /*mode=*/0) != 0 ||
        g_api.pcm_set_params(g_pcm, kFormatS16Le, kAccessRwInterleaved, /*channels=*/1, kRate,
                             /*soft_resample=*/1, kLatencyUs) != 0) {
        if (g_pcm != nullptr) {
            g_api.pcm_close(g_pcm);
            g_pcm = nullptr;
        }
        std::printf("alsa: unavailable (cannot open PCM device \"%s\")\n", device);
        return false;
    }

    pthread_t thread;
    if (pthread_create(&thread, nullptr, sound_thread, nullptr) != 0) {
        g_api.pcm_close(g_pcm);
        g_pcm = nullptr;
        std::printf("alsa: unavailable (cannot start audio thread)\n");
        return false;
    }
    pthread_detach(thread);

    std::printf("alsa: \"%s\" 48kHz mono S16\n", device);
    return true;
}

void Sound::play(SoundEffect effect) {
    if (!enabled_) {
        return;
    }
    // Same frequency/duration pairs as the vendored pwm_sound driver.
    switch (effect) {
        case SoundEffect::kBeep:
            enqueue(1000, kRate / 10);   // 100 ms
            break;
        case SoundEffect::kTabSwitch:
            enqueue(1500, kRate / 20);   // 50 ms
            break;
        case SoundEffect::kError:
            enqueue(400, kRate / 5);     // 200 ms
            break;
    }
}

bool Sound::tone(int freq_hz, int duration_ms) {
    if (!enabled_) {
        return false;
    }
    if (freq_hz <= 0 || duration_ms <= 0) {
        return false;
    }
    return enqueue(static_cast<uint32_t>(freq_hz),
                   static_cast<uint32_t>(static_cast<uint64_t>(kRate) * duration_ms / 1000));
}

void Sound::set_enabled(bool enabled) {
    enabled_ = enabled;
    if (!enabled) {
        pthread_mutex_lock(&g_mutex);
        g_queue_len = 0;
        g_current.frames = 0;
        pthread_mutex_unlock(&g_mutex);
    }
}

Sound& sound() {
    static Sound instance;
    return instance;
}

}  // namespace platform
