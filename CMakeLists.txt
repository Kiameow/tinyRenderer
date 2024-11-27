# 设置 CMake 的最低版本要求
cmake_minimum_required(VERSION 3.10)

# 设置项目名称
project(TinyRenderer)

# 指定 C++ 标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加可执行文件
add_executable(program
    main.cpp
    tgaimage.cpp
    line.cpp
    model.cpp
    geometry.cpp
    triangle.cpp
    global.cpp
)

# 如果需要链接外部库，可以在这里添加
# target_link_libraries(program some_library)
