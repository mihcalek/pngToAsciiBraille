cmake_minimum_required(VERSION 3.29)
project(brailleArt C)

set(CMAKE_C_STANDARD 11)

add_executable(brailleArt main.c
        stb_image.h
        stb_image_write.h)

target_link_libraries(brailleArt m)