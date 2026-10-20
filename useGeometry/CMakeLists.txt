cmake_minimum_required(VERSION 3.10)
project(useGeometry)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)

# 查找Eigen库（apt安装的可以直接找到）
find_package(Eigen3 REQUIRED)

# 生成可执行文件
add_executable(useGeometry useGeometry.cpp)

# 链接Eigen头文件（Eigen是纯头文件库，不需要链接库文件）
target_link_libraries(useGeometry PRIVATE Eigen3::Eigen)
