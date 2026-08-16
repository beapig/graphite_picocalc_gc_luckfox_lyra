// Host platform bring-up: SDL must be initialized before Display::init
// creates the window, so main() calls host_sdl_init() first, then
// platform::init().

#include "platform/platform.hpp"

#include <cstdlib>

#include <SDL2/SDL.h>

#include "gfx/framebuffer.hpp"

namespace platform {

InitStatus init() {
    InitStatus status;

    keyboard().init();  // No hardware; SDL feeds the queue
    status.keyboard = true;

    display().init();  // Creates the window + texture
    status.display = display().initialized();

    status.psram = psram().init();      // malloc arena
    status.storage = storage().init();  // data directory

    return status;
}

}  // namespace platform

namespace host {

// Defined in keyboard_sdl.cpp — translates an SDL key event into the
// platform KeyEvent stream.
void enqueue_sdl_key(const SDL_KeyboardEvent& ev);

// Quit flag: set by the mode screen's "Exit application" row. The main
// loop observes it via pump_sdl_events() and exits through the normal
// save-state path (same as closing the window).
bool g_exit_requested = false;

void request_exit() {
    g_exit_requested = true;
}

bool sdl_init() {
    // SDL is built without libudev, and in that configuration its evdev
    // core never scans /dev/input by itself (SDL_evdev.c: "TODO: Scan the
    // devices manually, like a caveman"). The KMSDRM backend would then
    // come up with a dead keyboard. Hand it the device explicitly:
    // class 2 is SDL_UDEV_DEVICE_KEYBOARD, and event0 is the board's
    // picocalc-keyboard (the only input node on this device). Harmless
    // under X11, whose backend reads keys from the X server instead.
    // overwrite=0 so the environment can override.
    setenv("SDL_EVDEV_DEVICES", "2:/dev/input/event0", 0);

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return false;
    }
    return true;
}

void sdl_shutdown() {
    SDL_Quit();
}

// SDL event pump. Window close / quit asks the main loop to exit.
bool pump_sdl_events() {
    if (g_exit_requested) {
        return false;
    }
    SDL_Event ev;
    while (SDL_PollEvent(&ev) != 0) {
        switch (ev.type) {
            case SDL_QUIT:
                return false;
            case SDL_KEYDOWN:
            case SDL_KEYUP:
                // repeat=1 KEYDOWNs are the evdev auto-repeat: the stock
                // firmware's MCU reported held keys as continuous "hold"
                // FIFO events, and this device's custom firmware instead
                // relies on the Linux input subsystem for key repeat. Pass
                // them through so held keys repeat in the UI.
                enqueue_sdl_key(ev.key);
                break;
            default:
                break;
        }
    }
    return true;
}

}  // namespace host
