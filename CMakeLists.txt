cmake_minimum_required(VERSION 3.0)
project(OhCrapImBlind)

set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS} -lsfml-graphics -lsfml-window -lsfml-system -g -ftest-coverage -fprofile-arcs -Wall -Wextra -Wno-unused -pedantic")
set(CMAKE_INCLUDE_CURRENT_DIR ON)
file(GLOB OhCrapImBlind_SRC
    "*.cpp"
    "*.hpp"
    )


set(EXECUTABLE_NAME "imblind")

add_executable(${EXECUTABLE_NAME} ${OhCrapImBlind_SRC})

add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)

# Detect and add SFML
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
#Find any version 2.X of SFML
#See the FindSFML.cmake file for additional details and instructions
find_package(SFML 2 REQUIRED system window graphics network audio)
if(SFML_FOUND)
  include_directories(${SFML_INCLUDE_DIR})
  target_link_libraries(${EXECUTABLE_NAME} ${SFML_LIBRARIES} "-lcAudio")
endif()


