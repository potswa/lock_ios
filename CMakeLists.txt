project(lock_ios)
cmake_minimum_required(VERSION 3.22.2)
set(FLAGS_ -Wall -Werror -fstack-protector)
file(GLOB SRC src/*.cpp)
add_library(lock_ios SHARED ${SRC})
target_include_directories(lock_ios PUBLIC ./)
target_compile_options(lock_ios PUBLIC ${FLAGS_})

