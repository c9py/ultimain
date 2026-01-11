cmake_minimum_required(VERSION 3.16)
project(UltimaLauncher VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SDL3
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL3 REQUIRED sdl3)

# Launcher executable
add_executable(ultima-launcher
    main.cpp
)

target_include_directories(ultima-launcher PRIVATE
    ${SDL3_INCLUDE_DIRS}
)

target_link_libraries(ultima-launcher PRIVATE
    ${SDL3_LIBRARIES}
)

target_compile_options(ultima-launcher PRIVATE
    ${SDL3_CFLAGS_OTHER}
)

# Install
install(TARGETS ultima-launcher RUNTIME DESTINATION bin)

message(STATUS "Ultima Launcher configured with SDL3 ${SDL3_VERSION}")
