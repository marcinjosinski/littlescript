cmake_minimum_required(VERSION 3.2)
project(littlescript LANGUAGES CXX)

set(SOURCE_FILES main.cxx)
add_executable(littlescript ${SOURCE_FILES})

set_target_properties(littlescript
  PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

target_compile_options(littlescript
  PRIVATE
    -Wall
	-Werror
	-Wpedantic
)
