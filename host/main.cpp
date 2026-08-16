// Host entry point — the firmware's main() with the hardware ritual
// removed and SDL in its place.
//
// Core loop shape is unchanged from the firmware: drain every queued
// key, run the power tick, service serial injection (stdin), render
// dirty bands. The late-init retry machinery for PSRAM/SD drop-out is
// gone: a malloc arena and a directory do not flake.

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>

#include <SDL2/SDL.h>

#include "config.hpp"
#include "platform/platform.hpp"
#include "platform/power.hpp"
#include "platform/sound.hpp"
#include "gfx/framebuffer.hpp"
#include "ui/chrome.hpp"
#include "ui/input_line.hpp"  // kCapacity bounds the serial-injection buffer
#include "ui/screen_manager.hpp"
#include "math/functions.hpp"
#include "math/lists.hpp"
#include "math/matrix.hpp"
#include "math/named_lists.hpp"
#include "apps/graph_model.hpp"
#include "apps/home_screen.hpp"

#ifndef PICOCALC_BUILD_ID
#define PICOCALC_BUILD_ID "unknown"
#endif
#ifndef PICOCALC_PHASE
#define PICOCALC_PHASE "?"
#endif

namespace host {

bool sdl_init();
void sdl_shutdown();
bool pump_sdl_events();
void enqueue_sdl_key(const SDL_KeyboardEvent& ev);
// Command hint table for the suggestion popup (compiled defaults +
// editable commands.txt); no-op table on the firmware side.
void init_command_hints();

}  // namespace host

namespace {

// Non-blocking stdin read, replacing the Pico SDK's
// getchar_timeout_us(0) in the serial-injection path.
int getchar_nowait() {
    unsigned char c = 0;
    const ssize_t n = ::read(STDIN_FILENO, &c, 1);
    return n == 1 ? c : -1;
}

}  // namespace

int main(int argc, char** argv) {
    // Phase 5.1 serial injection is opt-in: without --uart-inject stdin
    // is never read, and the SDL window is the only input path.
    bool uart_inject = false;
    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--uart-inject") == 0) {
            uart_inject = true;
        }
    }

    if (!host::sdl_init()) {
        return 1;
    }

    const platform::InitStatus init_status = platform::init();
    if (!init_status.display) {
        std::fprintf(stderr, "display init failed\n");
        host::sdl_shutdown();
        return 1;
    }
    std::printf("gc-host %s (phase %s) — data dir via $PICOCALC_DATA or ~/.picocalc\n",
                PICOCALC_BUILD_ID, PICOCALC_PHASE);
    if (uart_inject) {
        std::printf("uart-inject: reading lines from stdin\n");
    }

    math::fn::seed_rand(static_cast<std::uint64_t>(platform::uptime_us()));

    apps::home_screen().load_state();
    apps::load_graph_state();
    const bool lists_loaded = math::lists().load(platform::storage());
    const bool named_loaded = math::named_lists().load(platform::storage());
    const bool matrices_loaded = math::matrices().load(platform::storage());
    const bool matans_loaded = math::load_ans(platform::storage());
    const bool settings_loaded = platform::power::load(platform::storage());
    (void)lists_loaded;
    (void)named_loaded;
    (void)matrices_loaded;
    (void)matans_loaded;
    (void)settings_loaded;

    // Sound: beeps via ALSA when available; absence is not fatal.
    platform::sound().init();

    // Suggestion-popup command hints (defaults + editable commands.txt).
    // After the loads above so the data dir certainly exists.
    host::init_command_hints();

    auto& mgr = ui::screen_manager();
    mgr.push(&apps::home_screen());

    bool dirty = true;
    while (true) {
        if (!host::pump_sdl_events()) {
            break;  // Window closed
        }

        platform::power::tick();

        // Serial injection (Phase 5.1 protocol): lines on stdin are
        // submitted to the home screen and results echoed. Same shape
        // as the firmware's USB-serial path.
        if (uart_inject) {
            static char inject_buf[ui::InputLine::kCapacity];
            static size_t inject_len = 0;
            static bool inject_overflow = false;
            constexpr int kMaxInjectCharsPerFrame = 512;
            for (int i = 0; i < kMaxInjectCharsPerFrame; ++i) {
                const int c = getchar_nowait();
                if (c < 0) {
                    break;
                }
                if (c == '\r') {
                    continue;
                }
                if (c != '\n') {
                    if (inject_len + 1 < sizeof(inject_buf)) {
                        inject_buf[inject_len++] = static_cast<char>(c);
                    } else {
                        inject_overflow = true;
                    }
                    continue;
                }

                inject_buf[inject_len] = 0;
                const size_t line_len = inject_len;
                inject_len = 0;
                const bool was_overflow = inject_overflow;
                inject_overflow = false;

                if (was_overflow) {
                    std::printf("inject: error line too long (max %u)\n",
                                static_cast<unsigned>(sizeof(inject_buf) - 1));
                    continue;
                }
                if (line_len == 0) {
                    continue;
                }
                if (mgr.current() != &apps::home_screen()) {
                    mgr.pop_to_root();
                    std::printf("inject: popped to home\n");
                }

                const char* result = nullptr;
                const char* kind = nullptr;
                const std::uint64_t t0 = platform::uptime_us();
                const bool ok = apps::home_screen().submit_line(inject_buf, &result, &kind);
                const std::uint64_t elapsed_us = platform::uptime_us() - t0;
                if (!ok) {
                    std::printf("inject: error rejected \"%s\"\n", inject_buf);
                } else if (result == nullptr) {
                    std::printf("inject: \"%s\" -> command\n", inject_buf);
                } else {
                    std::printf("inject: \"%s\" -> \"%s\" kind=%s us=%lu\n", inject_buf, result,
                                kind, static_cast<unsigned long>(elapsed_us));
                }
                std::fflush(stdout);
                dirty = true;
            }
        }

        // Drain every queued key before rendering (firmware discipline:
        // held-key backlogs must play out within one frame).
        constexpr int kMaxEventsPerFrame = 16;
        for (int n = 0; n < kMaxEventsPerFrame;) {
            const platform::KeyEvent ev = platform::keyboard().poll();
            if (ev.key == platform::Key::kNone) {
                break;  // Queue drained (SDL events arrive per-pump)
            }
            ++n;
            if (!ev.pressed) {
                continue;
            }
            if (ev.key == platform::Key::kHome && mgr.current() != &apps::home_screen()) {
                mgr.pop_to_root();
            } else {
                mgr.handle_key(ev);
            }
            dirty = true;
        }

        // Status-bar liveness: repaint the band when battery changes.
        {
            static int last_batt_percent = -2;
            static bool last_batt_charging = false;
            const auto batt = platform::battery_poll();
            if (batt.percent != last_batt_percent || batt.charging != last_batt_charging) {
                last_batt_percent = batt.percent;
                last_batt_charging = batt.charging;
                if (ui::Screen* s = mgr.current()) {
                    s->invalidate_band(0, ui::kStatusBarH);
                }
                dirty = true;
            }
        }

        if (dirty) {
            mgr.render_frame();
            dirty = false;
        }

        SDL_Delay(4);  // ~240 Hz pump cap; vsync gates the real rate
    }

    platform::power::save(platform::storage());
    host::sdl_shutdown();
    return 0;
}
