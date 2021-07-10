cmake_minimum_required(VERSION 3.7)

project(dbuild)

set(THIRD_PARTY_INCLUDE_DIRS third_party)
set(SRC_DIRS src)

set(CMAKE_CXX_STANDARD 17)
file(GLOB SRCS ${SRC_DIRS}/*.cpp ${SRC_DIRS}/*.h)

add_executable(${CMAKE_PROJECT_NAME} ${SRCS})

# find_package(GLEW REQUIRED)
# find_package(SDL2 REQUIRED)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  set(add_link_deps Threads::Threads)
endif()

include_directories(${THIRD_PARTY_INCLUDE_DIRS})
target_link_libraries(${CMAKE_PROJECT_NAME} ${add_link_deps})