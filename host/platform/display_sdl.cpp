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

// Integer window scale that fits the desktop. The device panel is
// 320x320, exactly the native screen — a 2x window there gets scaled
// back down by the WM anyway, and any non-integer size the WM lands on
// (fluxbox remembers geometry) feeds the 320x320 logical image through
// the renderer's resampler. That resampler must be nearest-neighbor
// (see the hint below), but the window itself should still fit, so the
// common case is a clean 1:1 blit.
int window_scale() {
    SDL_DisplayMode dm{};
    if (SDL_GetDesktopDisplayMode(0, &dm) == 0 && dm.w >= kScreenW && dm.h >= kScreenH) {
        const int fit_w = dm.w / kScreenW;
        const int fit_h = dm.h / kScreenH;
        const int fit = fit_w < fit_h ? fit_w : fit_h;
        return fit < kScale ? fit : kScale;
    }
    return 1;
}

}  // namespace

void Display::init() {
    if (initialized_) {
        return;
    }
    // Nearest-neighbor scaling, set before anything creates a texture.
    // The 320x320 RGB565 image must never be bilinear-resampled: a
    // non-integer window size (WM fullscreen/resizes) then produces
    // alternating dim/bright columns — the "vertical interlace with
    // wrong colors" corruption — because adjacent texels get blended.
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

    // Force software-only window framebuffer under X11. Without this,
    // SDL_CreateWindowFramebuffer() (called during SDL_CreateRenderer)
    // tries to create an accelerated GLES renderer as a "texture
    // framebuffer" — see SDL_video.c SDL_CreateWindowTexture(). On
    // this device the GLES driver is emulated software (DRI3 error),
    // and the GLES renderer's ARGB8888 texture path corrupts the
    // 16-bit RGB565 rendering pipeline, producing alternating bright/
    // dark columns with wrong colors. KMSDRM does not have this
    // problem and must not disable the hint (it breaks the surface).
    if (SDL_GetCurrentVideoDriver() && SDL_strcmp(SDL_GetCurrentVideoDriver(), "x11") == 0) {
        SDL_SetHint(SDL_HINT_FRAMEBUFFER_ACCELERATION, "0");
    }

    const int scale = window_scale();
    // On a desktop that can't fit 2x (the device's own 320x320 panel),
    // go borderless-fullscreen: windowed, the WM titlebar pushes the
    // softkey bar past the bottom edge of the screen.
    const Uint32 fullscreen = (scale == 1) ? static_cast<Uint32>(SDL_WINDOW_FULLSCREEN_DESKTOP) : 0u;
    g_win = SDL_CreateWindow("PicoCalc Graph", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                             kScreenW * scale, kScreenH * scale,
                             SDL_WINDOW_RESIZABLE | fullscreen);
    if (g_win == nullptr) {
        SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
        return;
    }
    // Software renderer, pinned explicitly. The old SDL build had no GLES,
    // so asking for ACCELERATED silently fell back to software; with GLES
    // enabled (needed to compile in KMSDRM) the X11-EGL renderer gets
    // selected instead and its visual mismatches the window — vertical
    // interlace stripes and shifted colors. We rasterize RGB565 in
    // software anyway and 320x320 is trivial to blit, so software is the
    // right choice on every backend.
    g_ren = SDL_CreateRenderer(g_win, -1, SDL_RENDERER_SOFTWARE);
    if (g_ren == nullptr) {
        SDL_Log("SDL_CreateRenderer failed: %s", SDL_GetError());
        SDL_DestroyWindow(g_win);
        g_win = nullptr;
        return;
    }
    // Integer-ish scaling: renderer keeps 320x320 logical coordinates
    // regardless of the physical window size.
    SDL_RenderSetLogicalSize(g_ren, kScreenW, kScreenH);
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
