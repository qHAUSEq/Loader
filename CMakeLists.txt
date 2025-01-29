cmake_minimum_required(VERSION 3.14)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(CMAKE_GENERATOR_PLATFORM x64)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(PROJECT_NAME Loader)
project(${PROJECT_NAME})

set(SFML_DIR "${CMAKE_CURRENT_LIST_DIR}/external/SFML/lib/cmake/SFML")

file(GLOB ALL_REQUIRED_DLL_CMAKE_SFML "external/SFML/bin/*.dll")
file(COPY ${ALL_REQUIRED_DLL_CMAKE_SFML} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file(GLOB PROJECT_FILES "${CMAKE_CURRENT_LIST_DIR}/src/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/src/*.h" "${CMAKE_CURRENT_LIST_DIR}/src/test/*.cpp")
add_executable(${PROJECT_NAME} ${PROJECT_FILES})

find_package(SFML 2.5 COMPONENTS system window graphics network audio REQUIRED)
target_link_libraries(${PROJECT_NAME} sfml-audio sfml-graphics sfml-window sfml-system sfml-network)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/res/background.png
  DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Texture/)
  
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/res/NicoPaint-Monospaced.otf
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Font/)