# Graphite for Luckfox Lyra — the Linux port

A fork of [Graphite](README-upstream.md) ([upstream repo](https://github.com/moodoki/graphite_picocalc_gc)),
the TI-83/84-inspired graphing calculator, retargeted from bare-metal
Raspberry Pi Pico firmware to a **native Linux/X11 application** on the
[Luckfox Lyra](https://www.luckfox.com/) (Rockchip RK3506) handheld — the
PicoCalc-shaped Linux sibling of the Pico module.

The entire calculator — math engine, CAS, graphing, statistics, UI — runs
unchanged from the shared `src/` tree. Only the platform layer is new.

> For everything about Graphite itself — features, usage, roadmap,
> architecture, the original Pico builds — read the
> **[original README](README-upstream.md)**.

## What this fork adds

### The host port (`host/`)

A second build target that compiles the shared firmware tree as a
Linux SDL2 application, cross-built with the armv7 hard-float toolchain.

| Subsystem | Firmware (Pico SDK) | Host (this port) |
|---|---|---|
| Display | SPI + DMA to ST7365P | SDL2 software renderer, X11 window (RGB565 kept end-to-end) |
| Keyboard | STM32 co-processor, I2C | Linux evdev via SDL |
| Storage | FatFs on SD | POSIX files, `/picocalc/...` mapped to `~/.picocalc/` |
| Sound | PWM piezo | ALSA PCM (48 kHz square-wave tones) |
| Battery | ADC read | `/sys/firmware/picocalc/battery_percent`, charge-bit split (bit7 = charging, bits6-0 = percent) |
| Power | Sleep / bootloader reboot | Clean application exit |

Notable host details:

- **Zero link-time dependencies beyond libc/libstdc++/libm/pthread.**
  SDL2 is statically linked and X11 is resolved at runtime through
  SDL's `X11_SHARED` dlopen mechanism; ALSA is likewise dlopen'd at
  runtime. The binary drops onto the device as-is.
- **Custom-keyboard shift translation.** The stock keyboard firmware
  runs its own modifier state machine and reports cooked characters;
  a customized firmware (as used here) disables that and passes raw
  keycodes to Linux. The host backend therefore translates
  `keycode + KMOD_SHIFT` to the shifted character itself, matching the
  firmware's translation table byte-for-byte.
- **Key repeat.** Linux evdev owns auto-repeat; the host forwards SDL's
  repeated `KEYDOWN` events so holding a key repeats, like the firmware's
  hold events did.
- **Serial injection stays opt-in.** Phase 5.1's stdin line protocol is
  gated behind `--uart-inject`; by default the app just opens the GUI.
- **MODE screen adaptation.** "Reboot to bootloader" becomes
  "Exit application" (F3 MODE) — the app saves state and exits with rc 0.

### UI work (shared tree)

- **Wrap-around navigation everywhere.** All selectable-list screens —
  MODE, WINDOW, TBLSET, CONSTANTS, FILES, SOLVER, PLOT, DIST, the CALC and
  CAS menus, and the slot/list/matrix editors — cycle their selection
  modulo the row/column count in both directions; scrolling viewports
  follow the wrap. Table (infinite scroll) and help (text scroll) keep
  their semantics.
- **Yellow key accents.** Softkey prefixes (`3:MODE`) and secondary hint
  bars (`ENTER:EDIT`) render `KEY:` in a yellow accent with the action
  word in white, TI-style.
- **Input-line suggestion popup.** While typing, the trailing word is
  prefix-matched against the function catalog (`math::catalog`) and an
  editable command hint table; matches float above the input line.
  UP/DOWN cycle, ENTER completes, ESC dismisses. Command hints render in
  the same yellow accent to read apart from functions.
- **Command hint table (`host/platform/commands_file.cpp`).** The popup
  also matches commands (`cls`, `files`, `stats`, …). Compiled defaults
  mirror `handle_command()`, and `~/.picocalc/commands.txt` (auto-created
  on first run) overrides them: one `name<TAB>summary` per line, edit and
  relaunch, no rebuild.
- **Error beeps.** Evaluation errors play a short tone through the shared
  `platform::Sound` interface (ALSA here, PWM on firmware).

## Building

The host build cross-compiles on any x86-64 Linux with the armv7
toolchain. Two one-time setups: the cross libc, and a static SDL2.

### 1. Cross toolchain

```bash
# Ubuntu/Debian: g++-arm-linux-gnueabihf provides the cross libc/libstdc++
sudo apt install g++-arm-linux-gnueabihf cmake ninja-build
```

### 2. Static SDL2 + X11 stubs (once)

SDL2 is built from source with a minimal feature set (X11 video via
runtime dlopen, software renderer, **no audio/joystick/GL**) and
installed to `host/deps/sdl2-install`. The X11 headers and link stubs
come from the device's own packages and live in `host/sysroot` (pull
them once: `scp` the device's `/usr/include/X11` and the
`/usr/lib/arm-linux-gnueabihf/libX*.so` stubs over, or install
`libsdl2-dev` into a sysroot).

```bash
cd host
tar xf deps/SDL2-2.30.0.tar.gz -C deps   # if not already extracted
cmake -G Ninja -B deps/sdl2-build -S deps/SDL2-2.30.0 \
      -DCMAKE_TOOLCHAIN_FILE=$PWD/armv7-sdl-toolchain.cmake \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_INSTALL_PREFIX=$PWD/deps/sdl2-install \
      -DSDL_SHARED=OFF -DSDL_STATIC=ON \
      -DSDL_TEST=OFF -DSDL_OPENGL=OFF -DSDL_OPENGLES=OFF \
      -DSDL_AUDIO=OFF -DSDL_JOYSTICK=OFF -DSDL_HAPTIC=OFF \
      -DSDL_SENSOR=OFF -DSDL_RENDER=ON -DSDL_VIDEO=ON \
      -DSDL_X11=ON -DSDL_X11_SHARED=ON \
      -DSDL_CLOCK_GETTIME=ON -DSDL_LIBC=ON -DSDL_THREADS=ON \
      -DSDL_TIMERS=ON -DSDL_FILESYSTEM=ON -DSDL_LOADSO=ON
cmake --build deps/sdl2-build
cmake --install deps/sdl2-build
```

### 3. Build the app

```bash
cmake -G Ninja -B host/build -S host \
      -DCMAKE_TOOLCHAIN_FILE=$PWD/host/armv7-toolchain.cmake \
      -DCMAKE_BUILD_TYPE=Release
cmake --build host/build
# → host/build/gc-host
```

`host/deps/` and `host/sysroot/` are gitignored (the SDL source tree is
~100 MB); the toolchain files and CMake glue are committed.

### 4. Run on the device

```bash
scp host/build/gc-host user@lyra:~/gc-host
ssh user@lyra 'DISPLAY=:0 ~/gc-host'
```

Runtime knobs:

| Env / flag | Effect |
|---|---|
| `DISPLAY=:0` | Required — X11 app (no console/framebuffer path) |
| `--uart-inject` | Enable the stdin line protocol (`files`, `5+5`, …) for scripting |
| `PICOCALC_ALSA_DEVICE` | ALSA PCM for beeps (default `default`; e.g. `sysdefault:CARD=picocalcsndpwm`) |

Data lives in `~/.picocalc/` on the device (lists, matrices, history,
settings, and the editable `commands.txt`).

## Relationship to upstream

This fork tracks
[upstream Graphite](https://github.com/moodoki/graphite_picocalc_gc) and
shares its `src/` tree; the Pico firmware builds are untouched and still
build exactly as documented in the
[original README](README-upstream.md). The host port lives entirely in
`host/` plus small, `PICOCALC_HOST`-gated constants in the shared tree
(`src/config.hpp`, `src/gfx/framebuffer.cpp`).

## License

As upstream — project code MIT, vendored drivers GPL-2.0; see
[NOTICE.md](NOTICE.md). The host port's new code follows the same terms.
