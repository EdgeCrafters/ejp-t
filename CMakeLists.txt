cmake_minimum_required(VERSION 3.12)

project(
	EJS-T
	)

include(FetchContent)

FetchContent_Declare(
	libgit2	
	GIT_REPOSITORY "https://github.com/libgit2/libgit2.git"
	GIT_TAG "v1.6.4"
	)

FetchContent_Declare(
	libcurl
	GIT_REPOSITORY "https://github.com/curl/curl.git"
	GIT_TAG "curl-8_1_2"
	)

FetchContent_MakeAvailable(libgit2)
FetchContent_MakeAvailable(libcurl)

add_subdirectory(lib)
add_subdirectory(src)
add_subdirectory(test)

include(CTest)
add_test(test-0 test/test-main)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/workbook0/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/workbook0/)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/.ejs/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/.ejs/)


if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")
elseif(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /w")
endif()
