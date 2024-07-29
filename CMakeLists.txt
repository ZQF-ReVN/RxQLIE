# RxQLIE Main
cmake_minimum_required(VERSION 3.25)

# Project Name
project(Rvn_RxQLIE_Main)

# On Test Preset
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    include(CMakeMycfg.cmake)
    include(CMakeMypkg.cmake)
endif()

# Source
add_subdirectory(src)

# On Test Config
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
   # add_subdirectory(test)
endif()
