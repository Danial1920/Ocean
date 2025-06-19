cmake_minimum_required(VERSION 3.10)
project(OceanSimulation LANGUAGES CXX)

find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)


set(SOURCES
    main.cpp
    Ocean.cpp
    Sand.cpp
    Algae.cpp
    HerbivoreFish.cpp
    PredatorFish.cpp
)


add_executable(OceanSimulation ${SOURCES})

target_link_libraries(OceanSimulation
    SDL2::SDL2
    SDL2::SDL2main
    SDL2_ttf::SDL2_ttf
)

message(STATUS "Remember to place 'arial.ttf' in the same directory as the executable, or ensure it's available in system font paths for the application to run correctly.")