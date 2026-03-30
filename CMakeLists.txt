cmake_minimum_required(VERSION 3.23)

project(AudioEngine)


find_package(ALSA REQUIRED)

set(SOURCE_FILES 
   
    ./include/testprocess.h

    ./include/audioengine.h

    ./include/common/audioexception.h
    ./include/common/audioconfig.h
    ./include/common/audioprocess.h

    ./include/backends/audiobackend.h
    ./include/backends/backendfactory.h
    ./include/backends/linux/alsabackend.h
    ./include/devices/audiodevice.h

    ./src/testprocess.cpp
    ./src/main.cpp 
    ./src/backends/linux/alsabackend.cpp
    ./src/devices/audiodevice.cpp

)

add_executable(AUDIO_ENGINE ${SOURCE_FILES})
target_include_directories(AUDIO_ENGINE PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)


target_link_libraries(AUDIO_ENGINE PRIVATE ALSA::ALSA)