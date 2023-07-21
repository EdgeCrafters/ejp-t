cmake_minimum_required(VERSION 3.11)

project(
	EJS-T_APP
	LANGUAGES C
	)

include(FetchContent)

FetchContent_Declare(
	Libgit2	
	GIT_REPOSITORY "https://github.com/libgit2/libgit2.git"
	GIT_TAG "v1.7.0"
	)

FetchContent_Declare(
	Libcurl
	GIT_REPOSITORY "https://github.com/curl/curl.git"
	GIT_TAG "curl-8_1_2"
	)


FetchContent_MakeAvailable(Libgit2)
FetchContent_MakeAvailable(Libcurl)

include_directories(../lib)

add_executable(ejs-t main.c)

target_link_libraries( ejs-t PUBLIC parse sha256 diff http)
target_compile_options( ejs-t PUBLIC -w)
