cmake_minimum_required(VERSION 2.8.7)

project(Streams CXX)

# TODO: Make the following more granular once there's wider compiler support for
# c++14. Note that the absence of "-g" is intentional as the current version of
# clang (3.4) does not support emitting debug info for auto member functions.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++1y -stdlib=libc++ -Wno-unused-function -pipe")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -fno-inline")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O3 -march=native -DNDEBUG")

add_custom_target(linecount
  COMMAND wc -l source/*.h source/*/*.h | sort
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

enable_testing()

include_directories(source)
add_subdirectory(test)
