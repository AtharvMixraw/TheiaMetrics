cmake_minimum_required(VERSION 3.10)
project(VideoMetrics)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS} include)

add_executable(metrics src/main.cpp src/metrics.cpp)
target_link_libraries(metrics ${OpenCV_LIBS})
