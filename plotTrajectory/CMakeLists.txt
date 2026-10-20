cmake_minimum_required(VERSION 3.10)
project(plotTrajectory)

set(CMAKE_CXX_STANDARD 17)

# 查找依赖库
find_package(Eigen3 REQUIRED)
find_package(Pangolin REQUIRED)

add_executable(plotTrajectory plotTrajectory.cpp)

# 链接库
target_link_libraries(plotTrajectory PRIVATE
    Eigen3::Eigen
    ${Pangolin_LIBRARIES}
)
./build/plotTrajectory
