// Host PSRAM backend: the PicoCalc mainboard's 8 MB PSRAM does not
// exist on this target (100 MB DDR3 makes it redundant), but the
// calculator's large-dataset paths (lists, matrices) are sized against
// a PSRAM-like region. This backend keeps the exact interface and
// semantics — 32-bit offsets, bump allocation, 8 MB total — backed by
// a plain malloc arena.

#include "platform/psram.hpp"

#include <cstdlib>
#include <cstring>

namespace platform {

namespace {
uint8_t* g_arena = nullptr;
constexpr size_t kArenaBytes = 8u * 1024 * 1024;
}  // namespace

bool Psram::init() {
    if (ok_) {
        return true;
    }
    if (g_arena == nullptr) {
        g_arena = static_cast<uint8_t*>(std::calloc(kArenaBytes, 1));
    }
    ok_ = g_arena != nullptr;
    next_ = 0;
    return ok_;
}

bool Psram::reinit() {
    return init();
}

uint32_t Psram::alloc(size_t bytes, size_t alignment) {
    if (!ok_ || g_arena == nullptr) {
        return kInvalid;
    }
    const uint32_t aligned =
        static_cast<uint32_t>((next_ + (alignment - 1)) & ~(alignment - 1));
    if (aligned + bytes > kArenaBytes) {
        return kInvalid;
    }
    next_ = aligned + static_cast<uint32_t>(bytes);
    return aligned;
}

void Psram::reset() {
    next_ = 0;
}

void Psram::write_word(uint32_t addr, uint32_t value) {
    if (addr + 4 > kArenaBytes) {
        return;
    }
    std::memcpy(g_arena + addr, &value, 4);
}

uint32_t Psram::read_word(uint32_t addr) {
    if (addr + 4 > kArenaBytes) {
        return 0;
    }
    uint32_t v = 0;
    std::memcpy(&v, g_arena + addr, 4);
    return v;
}

void Psram::write(uint32_t addr, const uint8_t* data, size_t len) {
    if (addr >= kArenaBytes || addr + len > kArenaBytes) {
        return;
    }
    std::memcpy(g_arena + addr, data, len);
}

void Psram::read(uint32_t addr, uint8_t* data, size_t len) {
    if (addr >= kArenaBytes || addr + len > kArenaBytes) {
        return;
    }
    std::memcpy(data, g_arena + addr, len);
}

Psram& psram() {
    static Psram instance;
    return instance;
}

}  // namespace platform
