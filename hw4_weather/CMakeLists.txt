cmake_minimum_required(VERSION 3.14)

project(hw4 C)

set(PROGRAM_NAME weather)

set(SOURCES
		main.c
		utils.c)

set(HEADERS
		constants.h
		utils.h)


add_executable(${PROGRAM_NAME} ${SOURCES} ${HEADERS})

find_package(CURL REQUIRED)
target_include_directories(${PROGRAM_NAME} PRIVATE ${CURL_INCLUDE_DIR})
target_link_libraries(${PROGRAM_NAME} PRIVATE ${CURL_LIBRARIES})

find_package(cJSON REQUIRED)
target_include_directories(${PROGRAM_NAME} PRIVATE ${CJSON_INCLUDE_DIRS})
target_link_libraries(${PROGRAM_NAME} PRIVATE ${CJSON_LIBRARY})

if (CMAKE_C_COMPILER_ID MATCHES "Clang" OR
    CMAKE_C_COMPILER_ID STREQUAL "GNU")
    target_compile_options(${PROGRAM_NAME} PRIVATE 
        -Wall
        -Wextra
        -Werror
        -pedantic
    )
elseif (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(${PROGRAM_NAME} PRIVATE
        /EHsc
        /W4
        /WX
    )
endif()

set_target_properties(${PROGRAM_NAME}
	PROPERTIES
		C_STANDARD 17
		C_STANDARD_REQUIRED ON)
