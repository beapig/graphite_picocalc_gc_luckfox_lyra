// Host-only: the command hint table behind the input-line suggestion
// popup. The compiled defaults mirror handle_command()'s if-chain (add a
// command there -> add a row here); on top of them sits a user-editable
// override file, $PICOCALC_DATA/commands.txt ("name<TAB>summary" per
// line, '#'-comments and blanks ignored), auto-created with the defaults
// on first run. Edit the file -> relaunch -> the popup follows, no
// rebuild. The firmware build never links this file and leaves the
// table unset (see set_command_hints in home_screen.hpp).

#include <cstddef>
#include <cstdio>
#include <cstring>

#include "apps/home_screen.hpp"
#include "platform/storage.hpp"

namespace host {
namespace {

constexpr const char* kCommandsPath = "/picocalc/commands.txt";

// A summary may not exceed the popup's summary column (~34 chars on
// screen); names share the identifier charset the popup matches on.
constexpr size_t kNameCap = 15;
constexpr size_t kSummaryCap = 34;
constexpr int kMaxHints = 64;

struct RawHint {
    const char* name;
    const char* summary;
};

// Same order and aliases as handle_command(): primaries first, their
// short alias right after. '?' is unreachable (the popup's word charset
// is [a-z0-9_]) and so is deliberately absent.
constexpr RawHint kDefaults[] = {
    {"cls", "clear screen (keep hist)"},
    {"clrhist", "erase all history"},
    {"mode", "show/set modes"},
    {"help", "help browser"},
    {"files", "file list"},
    {"lists", "list editor"},
    {"list", "list editor"},
    {"stats", "statistics screen"},
    {"stat", "statistics screen"},
    {"dist", "distribution helper"},
    {"test", "inference tests"},
    {"infer", "inference tests"},
    {"plot", "stat plot setup"},
    {"plots", "stat plot setup"},
    {"matrix", "matrix editor"},
    {"mat", "matrix editor"},
    {"solve", "equation solver"},
    {"solver", "equation solver"},
    {"calc", "graph analysis menu"},
    {"analyze", "graph analysis menu"},
    {"const", "constants picker"},
    {"constants", "constants picker"},
    {"cas", "CAS menu"},
    {"settings", "device settings"},
    {"setup", "device settings"},
    {"diag", "diagnostics screen"},
};
constexpr int kDefaultCount = static_cast<int>(sizeof(kDefaults) / sizeof(kDefaults[0]));

// Static storage for the installed table: CommandHint rows pointing at
// these buffers stay valid for the program's lifetime (no heap, per the
// allocation rules — host platform code may use std::string, but the
// hints handed to the app must be stable pointers either way).
char g_names[kMaxHints][kNameCap + 1];
char g_sums[kMaxHints][kSummaryCap + 1];
apps::CommandHint g_table[kMaxHints];

bool valid_name(const char* s, size_t len) {
    if (len == 0 || len > kNameCap || s[0] < 'a' || s[0] > 'z') {
        return false;
    }
    for (size_t i = 0; i < len; ++i) {
        const char c = s[i];
        const bool ok = (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '_';
        if (!ok) {
            return false;
        }
    }
    return true;
}

void install_defaults() {
    int n = kDefaultCount < kMaxHints ? kDefaultCount : kMaxHints;
    for (int i = 0; i < n; ++i) {
        std::snprintf(g_names[i], sizeof(g_names[i]), "%s", kDefaults[i].name);
        std::snprintf(g_sums[i], sizeof(g_sums[i]), "%s", kDefaults[i].summary);
        g_table[i] = {g_names[i], g_sums[i]};
    }
    apps::set_command_hints(g_table, n);
}

void write_default_file() {
    char buf[4096];
    size_t off = 0;
    off += static_cast<size_t>(
        std::snprintf(buf + off, sizeof(buf) - off,
                      "# gc-host command hints for the input-line popup.\n"
                      "# One command per line: name<TAB>summary. Lines are matched\n"
                      "# by prefix while typing at the line start; edit freely.\n"));
    for (int i = 0; i < kDefaultCount && off < sizeof(buf) - (kNameCap + kSummaryCap + 4);
         ++i) {
        off += static_cast<size_t>(std::snprintf(buf + off, sizeof(buf) - off, "%s\t%s\n",
                                                 kDefaults[i].name, kDefaults[i].summary));
    }
    buf[off] = 0;
    platform::storage().write_string(kCommandsPath, buf);
}

// Returns how many rows parsed; caller falls back to the compiled
// defaults when the file exists but yields nothing usable.
int parse_file(const char* text) {
    int n = 0;
    const char* p = text;
    while (*p != 0 && n < kMaxHints) {
        const char* eol = p;
        while (*eol != 0 && *eol != '\n') {
            ++eol;
        }
        // Trim leading spaces, then skip blanks and '#'-comments.
        const char* line = p;
        while (line < eol && (*line == ' ' || line[0] == '\t')) {
            ++line;
        }
        if (line < eol && *line != '#') {
            const char* tab = static_cast<const char*>(
                std::memchr(line, '\t', static_cast<size_t>(eol - line)));
            if (tab != nullptr) {
                const size_t name_len = static_cast<size_t>(tab - line);
                const char* sum = tab + 1;
                while (sum < eol && *sum == ' ') {
                    ++sum;
                }
                size_t sum_len = static_cast<size_t>(eol - sum);
                while (sum_len > 0 && (sum[sum_len - 1] == '\r' || sum[sum_len - 1] == ' ')) {
                    --sum_len;
                }
                if (valid_name(line, name_len) && sum_len > 0) {
                    if (sum_len > kSummaryCap) {
                        sum_len = kSummaryCap;
                    }
                    std::memcpy(g_names[n], line, name_len);
                    g_names[n][name_len] = 0;
                    std::memcpy(g_sums[n], sum, sum_len);
                    g_sums[n][sum_len] = 0;
                    g_table[n] = {g_names[n], g_sums[n]};
                    ++n;
                }
            }
        }
        p = (*eol == 0) ? eol : eol + 1;
    }
    if (n > 0) {
        apps::set_command_hints(g_table, n);
    }
    return n;
}

}  // namespace

void init_command_hints() {
    // Storage is mounted in platform::init() before this runs.
    if (!platform::storage().mounted()) {
        install_defaults();
        return;
    }
    if (!platform::storage().file_exists(kCommandsPath)) {
        write_default_file();
        install_defaults();
        return;
    }
    static char file_buf[8192];  // tail of boot init, bss not stack
    if (platform::storage().read_string(kCommandsPath, file_buf, sizeof(file_buf))
        && parse_file(file_buf) > 0) {
        return;
    }
    install_defaults();
}

}  // namespace host
