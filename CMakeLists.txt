cmake_minimum_required(VERSION 3.30)
project(CPROJECT)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

FetchContent_Declare(
        fmt
        GIT_REPOSITORY  https://github.com/fmtlib/fmt
        GIT_TAG         11.1.4
)

set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++")

FetchContent_MakeAvailable(fmt)

add_executable(CPROJECT
        src/main.cpp
        src/Bmp.cpp
        src/Message.cpp
        src/Gif.cpp
        src/ApplicationManager.cpp)

target_include_directories(CPROJECT PRIVATE include src)

configure_file(src/helper.txt helper.txt COPYONLY)

target_link_libraries(CPROJECT fmt)
