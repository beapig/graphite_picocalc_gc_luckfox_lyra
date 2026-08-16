// Firmware sound backend: thin wrapper over the vendored pwm_sound
// driver. The driver's ISR and tone state are file-private to
// pwm_sound.c (vendored, read-only — D-prelude-1), so only its three
// fixed effects are reachable; arbitrary tones need a pwm_sound.h API
// change that Phase 6 (P6-13) has not settled.

#include "platform/sound.hpp"

extern "C" {
#include "pwm_sound.h"
}

namespace platform {

bool Sound::init() {
    sound_init();
    return true;
}

void Sound::play(SoundEffect effect) {
    if (!enabled_) {
        return;
    }
    switch (effect) {
        case SoundEffect::kBeep:
            sound_play(SND_BEEP);
            break;
        case SoundEffect::kTabSwitch:
            sound_play(SND_TAB_SWITCH);
            break;
        case SoundEffect::kError:
            sound_play(SND_ERROR);
            break;
    }
}

bool Sound::tone(int /*freq_hz*/, int /*duration_ms*/) {
    // Not reachable through the vendored driver's public API (P6-13).
    return false;
}

void Sound::set_enabled(bool enabled) {
    enabled_ = enabled;
    sound_set_enabled(enabled);
}

Sound& sound() {
    static Sound instance;
    return instance;
}

}  // namespace platform
