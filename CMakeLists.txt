cmake_minimum_required(VERSION 3.11)

project(
	EJS-T
	VERSION 0.11
	DESCRIPTION "Load to EJS-T"
	LANGUAGES C
	)

add_subdirectory(lib)

add_executable(ejs-t main.c)

target_link_libraries( ejs-t parse sha256)
