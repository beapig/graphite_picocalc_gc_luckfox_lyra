// Host storage backend: POSIX stdio over a data directory. Firmware
// paths are absolute FatFs paths ("/picocalc/foo"); here they are
// re-rooted under $PICOCALC_DATA or ~/.picocalc. "SD" from the
// firmware's perspective is just a directory on the root filesystem.

#include "platform/storage.hpp"

#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

namespace platform {

namespace {

std::string data_dir() {
    const char* env = std::getenv("PICOCALC_DATA");
    if (env != nullptr && env[0] != 0) {
        return env;
    }
    const char* home = std::getenv("HOME");
    if (home == nullptr || home[0] == 0) {
        home = ".";
    }
    return std::string(home) + "/.picocalc";
}

// "/picocalc/foo" or "picocalc/foo" -> "<data_dir>/foo"
std::string map_path(const char* path) {
    const char* p = path;
    while (*p == '/') {
        ++p;
    }
    // Strip a leading "picocalc" path segment if present (firmware layout).
    // Match both "picocalc/..." and the bare directory "/picocalc" itself,
    // which is what the FILES browser lists.
    if (std::strncmp(p, "picocalc/", 9) == 0) {
        p += 9;
    } else if (std::strcmp(p, "picocalc") == 0) {
        p += 8;
    }
    return data_dir() + "/" + p;
}

void ensure_parent_dirs(const std::string& file_path) {
    size_t pos = 0;
    while ((pos = file_path.find('/', pos + 1)) != std::string::npos) {
        ::mkdir(file_path.substr(0, pos).c_str(), 0755);
    }
}

}  // namespace

bool Storage::init() {
    ::mkdir(data_dir().c_str(), 0755);
    mounted_ = true;
    return true;
}

void Storage::on_card_removed() {
    // No hot-plug concept on a filesystem-backed store.
}

bool Storage::file_exists(const char* path) const {
    if (!mounted_) {
        return false;
    }
    struct stat st;
    return ::stat(map_path(path).c_str(), &st) == 0 && S_ISREG(st.st_mode);
}

long Storage::file_size(const char* path) const {
    if (!mounted_) {
        return -1;
    }
    struct stat st;
    if (::stat(map_path(path).c_str(), &st) != 0 || !S_ISREG(st.st_mode)) {
        return -1;
    }
    return static_cast<long>(st.st_size);
}

int Storage::read_file(const char* path, uint8_t* buf, size_t max_len) const {
    return read_file_range(path, 0, buf, max_len);
}

int Storage::read_file_range(const char* path, size_t offset, uint8_t* buf, size_t max_len) const {
    if (!mounted_) {
        return -1;
    }
    std::FILE* f = std::fopen(map_path(path).c_str(), "rb");
    if (f == nullptr) {
        return -1;
    }
    if (offset != 0 && std::fseek(f, static_cast<long>(offset), SEEK_SET) != 0) {
        std::fclose(f);
        return -1;
    }
    const size_t got = std::fread(buf, 1, max_len, f);
    std::fclose(f);
    return static_cast<int>(got);
}

bool Storage::write_file(const char* path, const uint8_t* buf, size_t len) const {
    if (!mounted_) {
        return false;
    }
    const std::string full = map_path(path);
    ensure_parent_dirs(full);
    std::FILE* f = std::fopen(full.c_str(), "wb");
    if (f == nullptr) {
        return false;
    }
    const size_t put = std::fwrite(buf, 1, len, f);
    std::fclose(f);
    return put == len;
}

bool Storage::append_file(const char* path, const uint8_t* buf, size_t len) const {
    if (!mounted_) {
        return false;
    }
    const std::string full = map_path(path);
    ensure_parent_dirs(full);
    std::FILE* f = std::fopen(full.c_str(), "ab");
    if (f == nullptr) {
        return false;
    }
    const size_t put = std::fwrite(buf, 1, len, f);
    std::fclose(f);
    return put == len;
}

bool Storage::delete_file(const char* path) const {
    if (!mounted_) {
        return false;
    }
    return std::remove(map_path(path).c_str()) == 0;
}

bool Storage::ensure_dir(const char* path) const {
    if (!mounted_) {
        return false;
    }
    const std::string full = map_path(path);
    if (::mkdir(full.c_str(), 0755) == 0) {
        return true;
    }
    struct stat st;
    return ::stat(full.c_str(), &st) == 0 && S_ISDIR(st.st_mode);
}

int Storage::list_dir(const char* path, DirEntry* entries, int max_entries) const {
    if (!mounted_) {
        return -1;
    }
    DIR* d = ::opendir(map_path(path).c_str());
    if (d == nullptr) {
        return -1;
    }
    int n = 0;
    struct dirent* de;
    while (n < max_entries && (de = ::readdir(d)) != nullptr) {
        if (de->d_name[0] == '.') {
            continue;
        }
        std::strncpy(entries[n].name, de->d_name, sizeof(entries[n].name) - 1);
        entries[n].name[sizeof(entries[n].name) - 1] = 0;
        struct stat st;
        if (::stat((map_path(path) + "/" + de->d_name).c_str(), &st) == 0) {
            entries[n].is_dir = S_ISDIR(st.st_mode);
            entries[n].size = static_cast<uint32_t>(st.st_size);
        } else {
            entries[n].is_dir = false;
            entries[n].size = 0;
        }
        ++n;
    }
    ::closedir(d);
    return n;
}

bool Storage::read_string(const char* path, char* buf, size_t max_len) const {
    if (max_len == 0) {
        return false;
    }
    const int n = read_file(path, reinterpret_cast<uint8_t*>(buf), max_len - 1);
    if (n < 0) {
        return false;
    }
    buf[n] = 0;
    return true;
}

bool Storage::write_string(const char* path, const char* str) const {
    return write_file(path, reinterpret_cast<const uint8_t*>(str), std::strlen(str));
}

Storage& storage() {
    static Storage instance;
    return instance;
}

}  // namespace platform
