cmake_minimum_required(VERSION 3.28)

project(BPlusTree)
set(CMAKE_CXX_STANDARD 17)

add_executable(
        BPlusTree
        src/main.c
        src/BPlusTree.h
        src/BPlusTree.c
)
