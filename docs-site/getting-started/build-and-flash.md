# Build and flash

How to build the firmware from source and flash it onto a PicoCalc.
Adapted from the main repository's `README.md` quick-start; kept in
sync with it by hand for now.

## Install dependencies (macOS)

```bash
brew install cmake ninja git python3 picotool
brew install --cask gcc-arm-embedded   # ARM GNU Toolchain -> /Applications/ArmGNUToolchain
```

Do **not** use the Homebrew *formula* `arm-none-eabi-gcc` — it ships
without newlib and fails at link time with
`nosys.specs: No such file or directory`. Use the cask above instead.

## Clone the repository and the Pico SDK

```bash
git clone <this-repo>
cd picocalc_gc
git clone -b 2.2.0 --recurse-submodules https://github.com/raspberrypi/pico-sdk.git
```

## Set environment variables

```bash
export PICO_SDK_PATH="$PWD/pico-sdk"
export PICO_TOOLCHAIN_PATH="/Applications/ArmGNUToolchain/15.2.rel1/arm-none-eabi"
```

Adjust the toolchain version path to whatever the cask installed.

## Build

```bash
./scripts/build-all.sh
# -> build/pico/picocalc_graphcalc.uf2
# -> build/pico2/picocalc_graphcalc.uf2
```

Pick the `.uf2` matching your board: **Pico 1 H (RP2040)** or
**Pico 2 H (RP2350)**.

## Flash

1. Hold **BOOTSEL** on the PicoCalc and plug in a USB-C cable.
2. Copy the `.uf2` onto the drive that appears:

   ```bash
   cp build/pico/picocalc_graphcalc.uf2 /Volumes/RPI-RP2/
   ```

3. The device reboots into the new firmware automatically.

Already running firmware and want to reflash without unplugging to find
BOOTSEL? From the running firmware: **Home -> F4 MODE -> "Reboot to
bootloader" -> ENTER**.

## Next

Continue to [First steps](first-steps.md) once the firmware is flashed.
For detailed toolchain troubleshooting, see the main repository's
`docs/dev-environment.md`.
