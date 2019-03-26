cmake_minimum_required(VERSION 3.0)

include("cmake/HunterGate.cmake")
HunterGate(
    URL "https://github.com/ruslo/hunter/archive/v0.23.111.tar.gz"
    SHA1 "4189bbd869abe4eb69b5303d3f916c03d34dc154"
)

project(semitas)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin")

hunter_add_package(glfw)
find_package(glfw3 REQUIRED)
hunter_add_package(glm)
find_package(glm REQUIRED)
hunter_add_package(ZLIB)
find_package(ZLIB CONFIG REQUIRED)
hunter_add_package(vorbis)
find_package(vorbis CONFIG REQUIRED)
hunter_add_package(fmt)
find_package(fmt CONFIG REQUIRED)
hunter_add_package(glog)
find_package(glog CONFIG REQUIRED)
hunter_add_package(entityx)
find_package(entityx CONFIG REQUIRED)

file(GLOB_RECURSE SRC_COMPENENTS
    "${CMAKE_SOURCE_DIR}/src/components/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/components/*.h"
)
file(GLOB_RECURSE SRC_GAME
    "${CMAKE_SOURCE_DIR}/src/game/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/game/*.h"
)
file(GLOB_RECURSE SRC_GLAD
    "${CMAKE_SOURCE_DIR}/src/glad/*.c"
    "${CMAKE_SOURCE_DIR}/src/glad/*.h"
)
file(GLOB_RECURSE SRC_SYSTEMS
    "${CMAKE_SOURCE_DIR}/src/systems/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/systems/*.h"
)
file(GLOB_RECURSE SRC_UTILS
    "${CMAKE_SOURCE_DIR}/src/utils/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/utils/*.h"
)
file(GLOB_RECURSE SRC_GRAPHICS
    "${CMAKE_SOURCE_DIR}/src/graphics/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/graphics/*.h"
)
file(GLOB_RECURSE SRC_SHADER
    "${CMAKE_SOURCE_DIR}/shader/*.frag"
    "${CMAKE_SOURCE_DIR}/shader/*.vert"
)

add_executable(semitas ${SRC_COMPENENTS} ${SRC_GAME} ${SRC_GLAD} ${SRC_SYSTEMS} ${SRC_GRAPHICS} ${SRC_UTILS})

source_group("components"            FILES ${SRC_COMPENENTS})
source_group("game"                  FILES ${SRC_GAME})
source_group("glad"                  FILES ${SRC_GLAD})
source_group("systems"               FILES ${SRC_SYSTEMS})
source_group("utils"                 FILES ${SRC_UTILS})
source_group("graphics"              FILES ${SRC_GRAPHICS})
source_group("shader"                FILES ${SRC_SHADER})

target_link_libraries(semitas glfw)
target_link_libraries(semitas glm)
target_link_libraries(semitas ZLIB::zlib)
target_link_libraries(semitas vorbis::vorbis)
target_link_libraries(semitas fmt::fmt)
target_link_libraries(semitas entityx::entityx)
target_link_libraries(semitas glog::glog)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	target_link_libraries(semitas c++experimental)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	target_link_libraries(semitas stdc++fs)
endif()

install(TARGETS semitas RUNTIME DESTINATION bin)
