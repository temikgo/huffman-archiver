cmake_minimum_required(VERSION 3.0)
project(archiver)

set(CMAKE_CXX_STANDARD 17)

add_executable(archiver
        main.cpp
        istreamwrapper.cpp
        istreamwrapper.h
        ostreamwrapper.cpp
        ostreamwrapper.h
        node.cpp
        node.h
        coder.cpp
        coder.h
        decoder.cpp
        decoder.h)
