# Versione minima di CMake richiesta
cmake_minimum_required(VERSION 3.10)

# Nome del progetto e linguaggio
project(rtracer LANGUAGES C)

add_link_options(-lm)

find_package(OpenMP)
if (OpenMP_C_FOUND)
    add_link_options(-fopenmp)
    add_compile_options(-fopenmp)
endif ()

find_package(OpenACC)
if (OpenACC_C_FOUND)
    add_link_options(-fopenacc)
    add_compile_options(-fopenacc)
endif ()

add_compile_options(-Wall -Wextra -Wpedantic -Werror)

set(CMAKE_C_FLAGS_DEBUG "-fsanitize=address -fsanitize=undefined -g")

# Enable colored output for Ninja
if ("${CMAKE_GENERATOR}" STREQUAL "Ninja")
    add_compile_options(-fdiagnostics-color=always)
endif ()

set(SOURCES
)

add_library(raytracing src/utils.h src/ray.h src/ray.c src/vec3.h src/shapes.h src/shapes.c
        src/world.h src/world.c src/hit_record.c src/hit_record.h src/material.h src/scatter.c
        src/camera.c src/camera.h)

# Definisci l'eseguibile da costruire
add_executable(rtracer src/main.c)
target_link_libraries(rtracer PRIVATE raytracing)
