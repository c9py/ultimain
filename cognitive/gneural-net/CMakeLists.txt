cmake_minimum_required(VERSION 3.16)
project(gneural LANGUAGES C)

# Static library for the programmable network. The script front-end
# (gneural_network.c and parser.c) stays an autotools program.
add_library(gneural STATIC
    src/activation.c
    src/binom.c
    src/error.c
    src/fact.c
    src/feedforward.c
    src/genetic_algorithm.c
    src/gradient_descent.c
    src/load.c
    src/msmco.c
    src/network.c
    src/random_search.c
    src/randomize.c
    src/rnd.c
    src/save.c
    src/simulated_annealing.c
)

target_include_directories(gneural PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# GNU89 inline emits the activation and rnd symbols other files call.
# includes.h pulls in string.h when this is set, and omp.h with the rest.
target_compile_definitions(gneural PRIVATE HAVE_STRING_H=1)
# The C sources use GNU89 inline and implicit declarations.
# Shared Library and NPC AI CMake jobs compile this library; C99 for-init
# in simulated_annealing.c is a hard error under this flag.
target_compile_options(gneural PRIVATE -std=gnu89 -fgnu89-inline)
target_link_libraries(gneural PUBLIC m)

find_package(OpenMP COMPONENTS C)
if(OpenMP_C_FOUND)
    target_link_libraries(gneural PUBLIC OpenMP::OpenMP_C)
else()
    # gcc provides omp.h via -fopenmp. Clang needs libomp-dev for the same flag.
    target_compile_options(gneural PRIVATE -fopenmp)
    target_link_options(gneural PUBLIC -fopenmp)
endif()
