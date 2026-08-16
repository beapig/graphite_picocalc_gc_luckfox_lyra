// Host keyboard backend: SDL window events translated into the same
// platform::KeyEvent stream the STM32 I2C protocol produces. The stock
// PicoCalc firmware applied modifiers on the MCU and sent translated
// characters; this device's customized firmware passes keys through
// raw (Linux maintains modifier state), so the shift translation the
// MCU used to do happens here (see shifted_char).
//
// main.cpp pumps SDL events and calls host::enqueue_sdl_key(); the
// Keyboard singleton drains that queue from poll().

#include "platform/keyboard.hpp"

#include <SDL2/SDL.h>

namespace platform {

namespace {

// Event queue between the SDL pump (main loop) and Keyboard::poll().
// Single-threaded by construction (both run on the main thread), so a
// plain ring buffer needs no locking.
constexpr int kQueueCap = 64;
KeyEvent g_queue[kQueueCap];
int g_q_head = 0;
int g_q_tail = 0;
int g_pending = 0;

Key key_from_sdl(SDL_Keycode k) {
    switch (k) {
        case SDLK_0: return Key::k0;
        case SDLK_1: return Key::k1;
        case SDLK_2: return Key::k2;
        case SDLK_3: return Key::k3;
        case SDLK_4: return Key::k4;
        case SDLK_5: return Key::k5;
        case SDLK_6: return Key::k6;
        case SDLK_7: return Key::k7;
        case SDLK_8: return Key::k8;
        case SDLK_9: return Key::k9;
        case SDLK_KP_0: return Key::k0;
        case SDLK_KP_1: return Key::k1;
        case SDLK_KP_2: return Key::k2;
        case SDLK_KP_3: return Key::k3;
        case SDLK_KP_4: return Key::k4;
        case SDLK_KP_5: return Key::k5;
        case SDLK_KP_6: return Key::k6;
        case SDLK_KP_7: return Key::k7;
        case SDLK_KP_8: return Key::k8;
        case SDLK_KP_9: return Key::k9;
        case SDLK_PERIOD: return Key::kDot;
        case SDLK_KP_PERIOD: return Key::kDot;
        case SDLK_PLUS: return Key::kPlus;
        case SDLK_KP_PLUS: return Key::kPlus;
        case SDLK_MINUS: return Key::kMinus;
        case SDLK_KP_MINUS: return Key::kMinus;
        case SDLK_ASTERISK: return Key::kMultiply;
        case SDLK_KP_MULTIPLY: return Key::kMultiply;
        case SDLK_SLASH: return Key::kDivide;
        case SDLK_KP_DIVIDE: return Key::kDivide;
        case SDLK_CARET: return Key::kPower;
        case SDLK_KP_POWER: return Key::kPower;
        case SDLK_LEFTPAREN: return Key::kLParen;
        case SDLK_RIGHTPAREN: return Key::kRParen;
        case SDLK_COMMA: return Key::kComma;
        case SDLK_KP_COMMA: return Key::kComma;
        case SDLK_EQUALS: return Key::kEquals;
        case SDLK_RETURN: return Key::kEnter;
        case SDLK_KP_ENTER: return Key::kEnter;
        case SDLK_BACKSPACE: return Key::kBackspace;
        case SDLK_DELETE: return Key::kDel;
        case SDLK_ESCAPE: return Key::kEscape;
        case SDLK_TAB: return Key::kTab;
        case SDLK_SPACE: return Key::kSpace;
        case SDLK_HOME: return Key::kHome;
        case SDLK_INSERT: return Key::kInsert;
        case SDLK_UP: return Key::kUp;
        case SDLK_DOWN: return Key::kDown;
        case SDLK_LEFT: return Key::kLeft;
        case SDLK_RIGHT: return Key::kRight;
        case SDLK_LSHIFT:
        case SDLK_RSHIFT: return Key::kShift;
        case SDLK_LCTRL:
        case SDLK_RCTRL: return Key::kCtrl;
        case SDLK_LALT:
        case SDLK_RALT: return Key::kAlt;
        default: break;
    }
    if (k >= SDLK_F1 && k <= SDLK_F10) {
        return static_cast<Key>(static_cast<int>(Key::kF1) + (k - SDLK_F1));
    }
    if (k >= SDLK_a && k <= SDLK_z) {
        return static_cast<Key>(static_cast<int>(Key::kA) + (k - SDLK_a));
    }
    return Key::kPrintable;
}

// US-ANSI shifted variants of the base keys. The customized keyboard
// firmware has its MCU-side modifier state machine disabled: keys pass
// through raw and the Linux input stack tracks modifiers, so SDL
// reports the base keysym plus KMOD_SHIFT. The firmware contract
// expects the shifted character (the stock STM32 firmware sent it
// pre-translated, e.g. shift+1 arriving as '!'), so the translation
// happens here. When the keysym already carries the shifted form (some
// X keymaps do), the table has no entry and it passes through intact.
char shifted_char(char c) {
    if (c >= 'a' && c <= 'z') {
        return static_cast<char>(c - 'a' + 'A');
    }
    switch (c) {
        case '1': return '!';
        case '2': return '@';
        case '3': return '#';
        case '4': return '$';
        case '5': return '%';
        case '6': return '^';
        case '7': return '&';
        case '8': return '*';
        case '9': return '(';
        case '0': return ')';
        case '`': return '~';
        case '-': return '_';
        case '=': return '+';
        case '[': return '{';
        case ']': return '}';
        case '\\': return '|';
        case ';': return ':';
        case '\'': return '"';
        case ',': return '<';
        case '.': return '>';
        case '/': return '?';
        default: return 0;
    }
}

// Same ASCII decode the firmware runs on the STM32's character stream
// (src/platform/keyboard.cpp) — derived Key must match for parity.
Key key_from_char(char c) {
    if (c >= '0' && c <= '9') {
        return static_cast<Key>(static_cast<int>(Key::k0) + (c - '0'));
    }
    if (c >= 'a' && c <= 'z') {
        return static_cast<Key>(static_cast<int>(Key::kA) + (c - 'a'));
    }
    if (c >= 'A' && c <= 'Z') {
        return static_cast<Key>(static_cast<int>(Key::kA) + (c - 'A'));
    }
    switch (c) {
        case '.': return Key::kDot;
        case '+': return Key::kPlus;
        case '-': return Key::kMinus;
        case '*': return Key::kMultiply;
        case '/': return Key::kDivide;
        case '^': return Key::kPower;
        case '(': return Key::kLParen;
        case ')': return Key::kRParen;
        case ',': return Key::kComma;
        case '=': return Key::kEquals;
        case ' ': return Key::kSpace;
        default: return Key::kPrintable;
    }
}

}  // namespace

void Keyboard::init() {}

KeyEvent Keyboard::poll() {
    KeyEvent none;
    if (g_pending == 0) {
        return none;
    }
    const KeyEvent ev = g_queue[g_q_head];
    g_q_head = (g_q_head + 1) % kQueueCap;
    --g_pending;

    if (ev.key != Key::kNone) {
        held_[static_cast<int>(ev.key)] = ev.pressed;
    }
    shift_held_ = (ev.key == Key::kShift) ? ev.pressed : shift_held_;
    ctrl_held_ = (ev.key == Key::kCtrl) ? ev.pressed : ctrl_held_;
    alt_held_ = (ev.key == Key::kAlt) ? ev.pressed : alt_held_;
    fifo_empty_ = (g_pending == 0);
    return ev;
}

bool Keyboard::is_held(Key k) const {
    return held_[static_cast<int>(k)];
}

Keyboard& keyboard() {
    static Keyboard instance;
    return instance;
}

}  // namespace platform

namespace host {

void enqueue_sdl_key(const SDL_KeyboardEvent& ev) {
    if (platform::g_pending >= platform::kQueueCap) {
        return;  // Drop, matching the firmware's bounded drain
    }
    platform::KeyEvent out;
    out.pressed = (ev.type == SDL_KEYDOWN);
    out.key = platform::key_from_sdl(ev.keysym.sym);
    // Modifiers via SDL's own state (covers held-but-unreported cases).
    out.shift_held = (ev.keysym.mod & KMOD_SHIFT) != 0;
    out.ctrl_held = (ev.keysym.mod & KMOD_CTRL) != 0;
    out.alt_held = (ev.keysym.mod & KMOD_ALT) != 0;

    // Printable keys resolve to an effective character (shift-aware)
    // and derive their Key from it, mirroring the firmware's ASCII
    // decode: shift+1 -> '!' arrives as kPrintable/'!', shift+a -> 'A'
    // as kA/'A' — exactly what the stock STM32 firmware sent.
    if (ev.keysym.sym >= 0x20 && ev.keysym.sym < 0x7F) {
        char ch = static_cast<char>(ev.keysym.sym);
        if (out.shift_held) {
            const char shifted = platform::shifted_char(ch);
            if (shifted != 0) {
                ch = shifted;
            }
        }
        out.key = platform::key_from_char(ch);
        out.ch = ch;
    } else if (out.key == platform::Key::kPrintable) {
        return;  // Unmapped printable (dead key, IME) — drop
    }
    platform::g_queue[platform::g_q_tail] = out;
    platform::g_q_tail = (platform::g_q_tail + 1) % platform::kQueueCap;
    ++platform::g_pending;
}

}  // namespace host
