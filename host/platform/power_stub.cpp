// Host power backend. The STM32 south bridge (backlight, APD) does not
// exist on this target — a window has no backlight, and the OS owns
// power management. Settings persistence is kept so the settings
// screen round-trips like it does on firmware.

#include "platform/power.hpp"

#include <cstdio>
#include <cstring>

#include "platform/storage.hpp"

namespace platform::power {

namespace {
Settings g_settings;
constexpr char kMagic[4] = {'P', 'C', 'S', '1'};
}  // namespace

Settings& settings() {
    return g_settings;
}

void request_apply() {
    // No hardware to push levels to.
}

void tick() {
    // No inactivity timer: the OS screensaver owns that job.
}

bool note_key(bool) {
    return false;  // Never asleep, no wake-key swallowing
}

bool asleep() {
    return false;
}

bool save(Storage& storage) {
    unsigned char buf[sizeof(kMagic) + sizeof(Settings)] = {0};
    std::memcpy(buf, kMagic, sizeof(kMagic));
    std::memcpy(buf + sizeof(kMagic), &g_settings, sizeof(Settings));
    return storage.write_file("/picocalc/settings.dat", buf, sizeof(buf));
}

bool load(Storage& storage) {
    unsigned char buf[sizeof(kMagic) + sizeof(Settings)];
    const int n = storage.read_file("/picocalc/settings.dat", buf, sizeof(buf));
    if (n != static_cast<int>(sizeof(buf))) {
        return false;
    }
    if (std::memcmp(buf, kMagic, sizeof(kMagic)) != 0) {
        return false;
    }
    std::memcpy(&g_settings, buf + sizeof(kMagic), sizeof(Settings));
    return true;
}

}  // namespace platform::power
