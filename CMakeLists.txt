cmake_minimum_required(VERSION 3.25)
project(frame_analyzer)

find_package(Geode REQUIRED)

add_geode_mod(reumeow.frame_analyzer
    src/main.cpp
)
