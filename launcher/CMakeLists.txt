cmake_minimum_required(VERSION 3.16)
project(UltimaLauncher VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SDL3 and SDL3_ttf
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL3 REQUIRED sdl3>=3.2.0)
pkg_check_modules(SDL3_TTF REQUIRED sdl3-ttf>=3.0.0)

# Launcher executable
add_executable(ultima-launcher
    main.cpp
)

target_include_directories(ultima-launcher PRIVATE
    ${SDL3_INCLUDE_DIRS}
    ${SDL3_TTF_INCLUDE_DIRS}
)

target_link_libraries(ultima-launcher PRIVATE
    ${SDL3_LIBRARIES}
    ${SDL3_TTF_LIBRARIES}
)

target_compile_options(ultima-launcher PRIVATE
    ${SDL3_CFLAGS_OTHER}
    ${SDL3_TTF_CFLAGS_OTHER}
)

# Define asset paths for the executable
target_compile_definitions(ultima-launcher PRIVATE
    LAUNCHER_ASSET_DIR="${CMAKE_INSTALL_PREFIX}/share/ultima-launcher"
    LAUNCHER_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
)

# Install targets
install(TARGETS ultima-launcher RUNTIME DESTINATION bin)
install(DIRECTORY assets/ DESTINATION share/ultima-launcher)

message(STATUS "Ultima Launcher configured with SDL3 ${SDL3_VERSION} and SDL3_ttf ${SDL3_TTF_VERSION}")
