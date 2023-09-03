cmake_minimum_required(VERSION 3.11)
project(MyGitProject)

# Fetch libgit2
include(FetchContent)

FetchContent_Declare(
	libcurl
	GIT_REPOSITORY "https://github.com/curl/curl.git"
	GIT_TAG "curl-8_1_2"
) 
FetchContent_MakeAvailable(libcurl)

set(BUILD_CLAR OFF CACHE BOOL "Build libgit2's Tests" FORCE)

# Add subdirectories
add_subdirectory(lib)
add_subdirectory(src)
add_subdirectory(test)

include(CTest)
add_test(test-0 test/test-main)
set_tests_properties(test-0 PROPERTIES LABELS "MyLabel")

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/workbook0/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/workbook0/)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/.ejp/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/.ejp/)


if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")
elseif(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /w")
endif()
