cmake_minimum_required(VERSION 2.8)

## Use the variable PROJECT_NAME for changing the target name
set( PROJECT_NAME "cubeshelldb" )

## Set our project name
project(${PROJECT_NAME})


set(CMAKE_POSITION_INDEPENDENT_CODE ON)
add_compile_options(-fPIC -O2 -w)


## Use all the *.cpp files we found under this folder for the project
FILE(GLOB SRCS "libcubeshelldb/src/*.cpp")


#set(CMAKE_VERBOSE_MAKEFILE ON)
## Define the executable
add_library(${PROJECT_NAME} SHARED ${SRCS})

include_directories(${PROJECT_NAME} "libcubeshelldb/include/" /usr/local/include/libmongoc-1.0 "/usr/local/include/libbson-1.0")

target_link_libraries(${PROJECT_NAME} ssl crypto mongoc-1.0)

install(TARGETS ${PROJECT_NAME}
        LIBRARY
          DESTINATION /usr/local/lib
          COMPONENT Libraries)
install(DIRECTORY include/ DESTINATION /usr/local/include)
