#pragma once

namespace platform {

// Fixed sound effects — the same set the vendored pwm_sound driver
// exposes (SND_BEEP / SND_TAB_SWITCH / SND_ERROR).
enum class SoundEffect {
    kBeep,      // 1000 Hz, 100 ms
    kTabSwitch, // 1500 Hz, 50 ms
    kError,     //  400 Hz, 200 ms
};

// Square-wave tone output.
//
// Firmware: GP26/27 PWM through the vendored pwm_sound driver.
// Host: standard ALSA PCM ("default", or $PICOCALC_ALSA_DEVICE) reached
// via runtime dlopen — libasound.so.2 is never a link-time dependency,
// the same trick SDL's X11 backend uses for libX11.
class Sound {
public:
    // Opens the output path. Returns false when audio is unavailable
    // (libasound missing, PCM open failed); play()/tone() then no-op.
    bool init();

    // Fire-and-forget fixed effect.
    void play(SoundEffect effect);

    // Arbitrary square-wave tone, enqueued (non-blocking). Returns
    // false when the backend cannot play it.
    bool tone(int freq_hz, int duration_ms);

    void set_enabled(bool enabled);
    bool is_enabled() const { return enabled_; }

private:
    bool enabled_ = true;
};

Sound& sound();

}  // namespace platform
