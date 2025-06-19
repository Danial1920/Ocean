cmake_minimum_required(VERSION 3.10)
project(OceanSimulation LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17) 
set(CMAKE_CXX_STANDARD_REQUIRED TRUE) 
set(CMAKE_CXX_EXTENSIONS OFF)

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

find_package(SDL2 REQUIRED)
if (SDL2_FOUND)
    message(STATUS "Found SDL2: ${SDL2_INCLUDE_DIRS}")
    include_directories(${SDL2_INCLUDE_DIRS})
else()
    message(FATAL_ERROR "SDL2 not found!")
endif()

find_package(SDL2_ttf REQUIRED)
if (SDL2_ttf_FOUND)
    message(STATUS "Found SDL2_ttf: ${SDL2_TTF_INCLUDE_DIRS}")
    include_directories(${SDL2_TTF_INCLUDE_DIRS})
else()
    message(FATAL_ERROR "SDL2_ttf not found!")
endif()

add_executable(OceanSimulation
               src/main.cpp
               src/Ocean.cpp
               src/Sand.cpp
               src/Algae.cpp
               src/HerbivoreFish.cpp
               src/PredatorFish.cpp
)

target_link_libraries(OceanSimulation
    ${SDL2_LIBRARIES}
    ${SDL2_TTF_LIBRARIES}
)


target_link_libraries(OceanSimulation
    SDL2::SDL2
    SDL2::SDL2main
    SDL2_ttf::SDL2_ttf
)

message(STATUS "Remember to place 'arial.ttf' in the same directory as the executable, or ensure it's available in system font paths for the application to run correctly.")