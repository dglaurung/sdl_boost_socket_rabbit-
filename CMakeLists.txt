cmake_minimum_required(VERSION 3.14)
project(sdl_boost_socket_rabbit)

set(CMAKE_CXX_STANDARD 17)

find_package(SDL REQUIRED)
find_package(SDL_ttf REQUIRED)
find_package(SDL_image REQUIRED)
include_directories( ${SDL_INCLUDE_DIR} )

find_package(Boost COMPONENTS system filesystem regex REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})

file(COPY freesansbold.ttf DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY cheese.ttf DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY babel.ttf DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY background.png DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_executable(sdl_boost_socket_rabbit main.cpp)
target_link_libraries(sdl_boost_socket_rabbit ${Boost_LIBRARIES} ${SDL_LIBRARY} ${SDL_TTF_LIBRARY} ${SDL_IMAGE_LIBRARY} -lm)