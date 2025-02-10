cmake_minimum_required(VERSION 3.5.0)
project(NURingIntegratedController VERSION 0.1.0 LANGUAGES C CXX)

# Set the C++ standard to use (for example, C++17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenCV REQUIRED)			

include_directories(${OpenCV_INCLUDE_DIRS})	

add_executable(NURingIntegratedController main.cpp)

target_link_libraries(NURingIntegratedController ${OpenCV_LIBS})
