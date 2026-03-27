cmake_minimum_required(VERSION 3.23)

project(AudioEngine)


find_package(ALSA REQUIRED)

set(SOURCE_FILES 
    ./src/main.cpp 
    ./src/backends/linux/alsabackend.cpp

    ./include/backends/audiobackend.h
    ./include/backends/linux/alsabackend.h
    ./include/devices/audiodevice.h

)

add_executable(AUDIO_ENGINE ${SOURCE_FILES})
target_include_directories(AUDIO_ENGINE PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)


