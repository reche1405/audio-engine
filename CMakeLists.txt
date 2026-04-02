cmake_minimum_required(VERSION 3.23)

project(AudioEngine)



set(SOURCE_FILES 
   
    ./include/testprocess.h

    ./include/audioengine.h

    ./include/common/audioexception.h
    ./include/common/audioconfig.h
    ./include/common/audioprocess.h

    ./include/backends/audiobackend.h
    ./include/backends/backendfactory.h
    ./include/devices/audiodevice.h
    
    ./src/testprocess.cpp
    ./src/main.cpp 
    ./src/devices/audiodevice.cpp
    
    )
if (UNIX) 
    set(SOURCE_FILES ${SOURCE_FILES}
        ./include/backends/linux/alsabackend.h
        ./src/backends/linux/alsabackend.cpp
    )
endif (UNIX)

if (WIN32)
     set(SOURCE_FILES ${SOURCE_FILES}
        ./include/backends/windows/wasapibackend.h
        ./src/backends/windows/wasapibackend.cpp
    )
endif (WIN32)
add_executable(AUDIO_ENGINE ${SOURCE_FILES})
target_include_directories(AUDIO_ENGINE PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

if (UNIX) 
    find_package(ALSA REQUIRED)

    target_link_libraries(AUDIO_ENGINE PRIVATE ALSA::ALSA)
endif(UNIX)