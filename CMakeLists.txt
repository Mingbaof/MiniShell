cmake_minimum_required(VERSION 3.26)
project(a1)

set(CMAKE_CXX_STANDARD 17)

add_executable(a1 main.cpp
        signal_handler.cpp
        signal_handler.h
        process_handler.cpp
        process_handler.h
        process.cpp
        process.h
        command.cpp
        command.h
        util.cpp
        util.h)
