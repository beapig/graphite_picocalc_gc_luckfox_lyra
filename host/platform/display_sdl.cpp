// Host display backend: 320x320 RGB565 software framebuffer -> SDL2
// window. The framebuffer's push_rect() contract is a blocking
// sub-rectangle upload, which maps directly onto SDL_UpdateTexture.
//
// The window is a normal desktop citizen (X11/fluxbox multitasking);
// there is no panel bring-up, no RGB666 conversion, no backlight —
// those live in the firmware's SPI transport.

#include "platform/display.hpp"

#include <SDL2/SDL.h>

namespace platform {

namespace {

SDL_Window* g_win = nullptr;
SDL_Renderer* g_ren = nullptr;
SDL_Texture* g_tex = nullptr;

constexpr int kScale = 2;  // 8x16 font is small at 1:1 on desktop panels

}  // namespace

void Display::init() {
    if (initialized_) {
        return;
    }
    g_win = SDL_CreateWindow("PicoCalc Graph", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                             kScreenW * kScale, kScreenH * kScale,
                             SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    if (g_win == nullptr) {
        SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
        return;
    }
    g_ren = SDL_CreateRenderer(g_win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (g_ren == nullptr) {
        // Software fallback still renders fine at this resolution.
        g_ren = SDL_CreateRenderer(g_win, -1, 0);
    }
    if (g_ren == nullptr) {
        SDL_Log("SDL_CreateRenderer failed: %s", SDL_GetError());
        SDL_DestroyWindow(g_win);
        g_win = nullptr;
        return;
    }
    // Integer-ish scaling: renderer keeps 320x320 logical coordinates
    // regardless of the physical window size.
    SDL_RenderSetLogicalSize(g_ren, kScreenW, kScreenH);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
    g_tex = SDL_CreateTexture(g_ren, SDL_PIXELFORMAT_RGB565, SDL_TEXTUREACCESS_STREAMING, kScreenW,
                              kScreenH);
    if (g_tex == nullptr) {
        SDL_Log("SDL_CreateTexture failed: %s", SDL_GetError());
        SDL_DestroyRenderer(g_ren);
        SDL_DestroyWindow(g_win);
        g_ren = nullptr;
        g_win = nullptr;
        return;
    }
    initialized_ = true;
}

void Display::push_rect(int x, int y, int w, int h, const uint16_t* px) {
    if (!initialized_ || w <= 0 || h <= 0) {
        return;
    }
    const SDL_Rect r{x, y, w, h};
    // px points at row (0) of the band; row stride is a full screen row.
    SDL_UpdateTexture(g_tex, &r, px, kScreenW * sizeof(uint16_t));
    SDL_RenderClear(g_ren);
    SDL_RenderCopy(g_ren, g_tex, nullptr, nullptr);
    SDL_RenderPresent(g_ren);
}

void Display::push_rect_dma(int x, int y, int w, int h, const uint16_t* px) {
    push_rect(x, y, w, h, px);
}

void Display::set_backlight(uint8_t) {
    // No backlight on a window.
}

Display& display() {
    static Display instance;
    return instance;
}

}  // namespace platform
