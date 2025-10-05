cmake_minimum_required(VERSION 3.10)
project(VideoQualityDashboard)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find OpenCV
find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS} include)

# Metrics calculator (original tool)
add_executable(metrics 
    src/main.cpp 
    src/metrics.cpp
)
target_link_libraries(metrics ${OpenCV_LIBS})

# Interactive dashboard (Phase 3)
add_executable(dashboard
    src/main_dashboard.cpp
    src/dashboard.cpp
    src/heatmap.cpp
    src/metrics.cpp
)
target_link_libraries(dashboard ${OpenCV_LIBS})

# Installation
install(TARGETS metrics dashboard DESTINATION bin)