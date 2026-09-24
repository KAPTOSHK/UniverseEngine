cmake_minimum_required(VERSION 3.10)
project(UniverseEngine CXX)

# Стандарт C++ (для старых MinGW лучше 11 или 14)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Если собираете из-под MinGW, желательно отключить префикс lib
set(CMAKE_EXECUTABLE_SUFFIX ".exe")

# Собираем исполняемый файл из main.cpp
add_executable(UniverseEngine WIN32 
    main.cpp
    InitGame.cpp
    Config.cpp
)

# Линкуем библиотеки Windows / OpenGL
target_link_libraries(UniverseEngine
    opengl32
    glu32
    gdi32
    winmm
    user32
)