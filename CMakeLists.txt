cmake_minimum_required(VERSION 3.11)

project(
	EJS-T
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

add_subdirectory(lib)
add_subdirectory(src)

set(testcase ${CMAKE_CURRENT_SOURCE_DIR}/testcase)

#configure_file("${CMAKE_CURRENT_SOURCE_DIR}/testcase/a.input" "${CMAKE_BINARY_DIR}" COPYONLY)

#add_custom_command(
#	TARGET ${PROJECT_NAME} POST_BUILD
#	COMMAND ${CMAKE_COMMAND} -E copy -R
#					${CMAKE_CURRENT_SOURCE_DIR}/testcase
#					${CMAKE_CURRENT_BINARY_DIR}
#	)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/testcase/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/testcase/)
