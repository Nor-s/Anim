cmake_minimum_required(VERSION 3.0)
project(Glad)

add_library(glad glad/glad.h KHR/khrplatform.h glad.c )
target_include_directories(glad PUBLIC ./)