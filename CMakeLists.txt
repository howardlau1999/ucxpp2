cmake_minimum_required(VERSION 3.3 FATAL_ERROR)
project(ucxpp)
set(CMAKE_CXX_STANDARD 20)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
find_package(Threads REQUIRED)
find_package(ucx REQUIRED)

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif ()

string(LENGTH "${CMAKE_SOURCE_DIR}/" SOURCE_PATH_LENGTH)
add_definitions("-DSOURCE_PATH_LENGTH=${SOURCE_PATH_LENGTH}")

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  option(UCXPP_BUILD_EXAMPLES "Build examples" ON)
else()
  option(UCXPP_BUILD_EXAMPLES "Build examples" OFF)
endif()

set(UCXPP_SOURCE_FILES 
  src/awaitable.cc
  src/context.cc
  src/worker.cc
  src/endpoint.cc
  src/address.cc
  src/memory.cc
  src/config.cc
)

add_library(ucxpp STATIC ${UCXPP_SOURCE_FILES})
set(UCXPP_LINK_LIBRARIES ucx::ucp ucx::uct ucx::ucs Threads::Threads)
list(APPEND
  UCXPP_COMPILE_OPTIONS
  PRIVATE
  -fno-rtti
  -Wall 
  -Wextra 
  -pedantic 
  -Werror
)
list(APPEND
  UCXPP_LINK_OPTIONS
  PRIVATE
  -fno-rtti
)

if (CMAKE_CXX_COMPILER_ID MATCHES Clang)
  list(APPEND
    UCXPP_COMPILE_OPTIONS
    PUBLIC
    -stdlib=libc++
    PUBLIC
    -fcoroutines-ts
  )
  list(APPEND
    UCXPP_LINK_OPTIONS
    PUBLIC
    -stdlib=libc++
  )
elseif (CMAKE_CXX_COMPILER_ID MATCHES GNU)
  if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS "11") 
    list(APPEND
      UCXPP_COMPILE_OPTIONS
      PUBLIC
      -fcoroutines
    )
  endif ()
endif ()
if (UCXPP_COMPILE_OPTIONS)
  target_compile_options(ucxpp ${UCXPP_COMPILE_OPTIONS})
endif ()
if (UCXPP_LINK_OPTIONS)
  target_link_options(ucxpp ${UCXPP_LINK_OPTIONS})
endif ()
target_link_libraries(ucxpp ${UCXPP_LINK_LIBRARIES})
target_include_directories(ucxpp PUBLIC include)