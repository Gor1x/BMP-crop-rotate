cmake_minimum_required(VERSION 3.15)
project(HW_01 C)

#set(flags "-Werror -Wno-unused-variable -Wall -Wextra -pedantic")
set(CMAKE_C_STANDARD 99)
SET(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} ${flags}")


add_executable(HW_01 main.c bmp.h bmp.c stego.h stego.c)