# Linux/SDL host build for the PicoCalc Graphing Calculator.
#
# Shares src/ and drivers/ (tinyexpr, cephes) with the firmware tree;
# only the platform backends are host-specific. Cross-compile for
# armv7 (Luckfox Lyra) with -DCMAKE_TOOLCHAIN_FILE=armv7-toolchain.cmake.

cmake_minimum_required(VERSION 3.16)

project(gc_host C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_definitions(PICOCALC_HOST=1)

add_compile_options(
    -Wall
    -Wextra
    -fno-exceptions
    $<$<COMPILE_LANGUAGE:CXX>:-Wnon-virtual-dtor>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-threadsafe-statics>
)

# ---- Shared sources (firmware tree, target-agnostic) ----
set(PORTABLE_SOURCES
    ../src/gfx/font.cpp
    ../src/gfx/framebuffer.cpp
    ../src/ui/screen_manager.cpp
    ../src/ui/input_line.cpp
    ../src/ui/chrome.cpp
    ../src/math/engine.cpp
    ../src/math/functions.cpp
    ../src/math/format.cpp
    ../src/math/frac.cpp
    ../src/math/autoclose.cpp
    ../src/math/catalog.cpp
    ../src/math/scratch.cpp
    ../src/math/array.cpp
    ../src/math/array_backend_pico.cpp
    ../src/math/lists.cpp
    ../src/math/lists_persist.cpp
    ../src/math/named_lists.cpp
    ../src/math/named_lists_persist.cpp
    ../src/math/list_ops.cpp
    ../src/math/stats.cpp
    ../src/math/dist.cpp
    ../src/math/infer.cpp
    ../src/math/matrix.cpp
    ../src/math/matrices_persist.cpp
    ../src/math/numeric_solve.cpp
    ../src/math/solve_expr.cpp
    ../src/math/complex.cpp
    ../src/math/unified_compile.cpp
    ../src/math/unified_vm.cpp
    ../src/math/unified_home.cpp
    ../src/math/array_format.cpp
    ../src/math/seq_expr.cpp
    ../src/math/units.cpp
    ../src/math/cas/expr.cpp
    ../src/math/cas/parser.cpp
    ../src/math/cas/serialize.cpp
    ../src/math/cas/simplify.cpp
    ../src/math/cas/derivative.cpp
    ../src/math/cas/expand.cpp
    ../src/math/cas/poly.cpp
    ../src/math/cas/solve.cpp
    ../src/math/cas/factor.cpp
    ../src/math/cas/integrate.cpp
    ../src/math/cas/cas_eval.cpp
    ../src/math/cas/exact.cpp
    ../src/render/pool.cpp
    ../src/render/layout_builder.cpp
    ../src/render/layout_render.cpp
    ../src/graph/viewport.cpp
    ../src/graph/plotter.cpp
    ../src/graph/graph_mode.cpp
    ../src/graph/graph_state.cpp
    ../src/graph/graph_persist.cpp
    ../src/graph/function_source.cpp
    ../src/graph/parametric_source.cpp
    ../src/graph/polar_source.cpp
    ../src/graph/seq_points.cpp
    ../src/graph/stat_plot.cpp
    ../src/graph/trace.cpp
    ../src/graph/analysis.cpp
    ../src/graph/analysis_cursor.cpp
    ../src/apps/nav.cpp
    ../src/apps/home_screen.cpp
    ../src/apps/const_screen.cpp
    ../src/apps/settings_screen.cpp
    ../src/apps/files_screen.cpp
    ../src/apps/list_editor.cpp
    ../src/apps/calc_menu.cpp
    ../src/apps/cas_menu.cpp
    ../src/apps/matrix_editor.cpp
    ../src/apps/solver_screen.cpp
    ../src/apps/stats_screen.cpp
    ../src/apps/dist_screen.cpp
    ../src/apps/infer_screen.cpp
    ../src/apps/plot_screen.cpp
    ../src/apps/graph_model.cpp
    ../src/apps/slot_editor.cpp
    ../src/apps/y_editor.cpp
    ../src/apps/param_editor.cpp
    ../src/apps/polar_editor.cpp
    ../src/apps/seq_editor.cpp
    ../src/apps/graph_screen.cpp
    ../src/apps/window_screen.cpp
    ../src/apps/mode_screen.cpp
    ../src/apps/help_screen.cpp
    ../src/apps/table_model.cpp
    ../src/apps/table_setup.cpp
    ../src/apps/table_screen.cpp
    ../src/apps/split_screen.cpp
)

add_executable(gc-host
    main.cpp
    platform/display_sdl.cpp
    platform/keyboard_sdl.cpp
    platform/psram_arena.cpp
    platform/storage_posix.cpp
    platform/system_linux.cpp
    platform/power_stub.cpp
    platform/sound_alsa.cpp
    platform/commands_file.cpp
    platform/fault_stub.cpp
    platform/platform_host.cpp
    ${PORTABLE_SOURCES}
)

target_include_directories(gc-host PRIVATE
    ../src
    ../drivers
)

# ---- tinyexpr (C99, TE_POW_FROM_RIGHT: TI-style right-assoc '^') ----
add_library(tinyexpr_host STATIC ../drivers/tinyexpr/tinyexpr.c)
target_include_directories(tinyexpr_host PUBLIC ../drivers/tinyexpr)
target_compile_definitions(tinyexpr_host PUBLIC TE_POW_FROM_RIGHT)

# ---- Cephes special functions (renamed symbols avoid libm clashes) ----
add_library(cephes_host STATIC
    ../drivers/cephes/const.c
    ../drivers/cephes/polevl.c
    ../drivers/cephes/mtherr.c
    ../drivers/cephes/gamma.c
    ../drivers/cephes/ndtr.c
    ../drivers/cephes/expx2.c
    ../drivers/cephes/ndtri.c
    ../drivers/cephes/igam.c
    ../drivers/cephes/igami.c
    ../drivers/cephes/incbet.c
    ../drivers/cephes/incbi.c
    ../src/math/cephes_support.c
)
target_compile_definitions(cephes_host PRIVATE
    gamma=cephes_gamma erf=cephes_erf erfc=cephes_erfc
)
target_compile_options(cephes_host PRIVATE -w)

# ---- SDL2: statically linked, built from source by the cross toolchain
# (host/deps/sdl2-install; see host/armv7-sdl-toolchain.cmake). Minimal
# feature set — X11 video via runtime dlopen, software renderer, no
# audio/joystick/GL — so the executable carries SDL with zero extra
# link-time dependencies. X11 and friends resolve on the device at
# runtime through SDL's X11_SHARED mechanism.
set(SDL2_DIR ${CMAKE_SOURCE_DIR}/deps/sdl2-install/lib/cmake/SDL2)
find_package(SDL2 REQUIRED CONFIG)

target_link_libraries(gc-host
    tinyexpr_host
    cephes_host
    SDL2::SDL2-static
    m
    pthread
)

# Build id (same convention as the firmware build).
execute_process(
    COMMAND git rev-parse --short HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/..
    OUTPUT_VARIABLE GIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)
if(NOT GIT_HASH)
    set(GIT_HASH "nogit")
endif()
execute_process(
    COMMAND git status --porcelain
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/..
    OUTPUT_VARIABLE GIT_DIRTY
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)
if(GIT_DIRTY)
    set(PICOCALC_BUILD_ID "${GIT_HASH}-dev")
else()
    set(PICOCALC_BUILD_ID "${GIT_HASH}")
endif()
target_compile_definitions(gc-host PRIVATE
    PICOCALC_BUILD_ID="${PICOCALC_BUILD_ID}"
    PICOCALC_PHASE="host"
    PICOCALC_SERIAL_INJECT=1
)
