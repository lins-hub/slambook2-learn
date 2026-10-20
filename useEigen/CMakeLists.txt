cmake_minimum_required(VERSION 3.10)
project(EigenMatrix)

# 添加头文件 (这就是为什么Eigen不需要target_link_libraries)
include_directories("/usr/include/eigen3")

add_executable(eigenMatrix eigenMatrix.cpp)