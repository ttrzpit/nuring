cmake_minimum_required(VERSION 3.5.0)
project(NURingIntegratedController VERSION 0.1.0 LANGUAGES C CXX)

# Set the C++ standard to use (for example, C++17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


# Find necessary pacakges 
find_package(OpenCV REQUIRED)	
find_package(X11 REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})	

add_executable(NURingIntegratedController main.cpp)


# Link libraries
target_link_libraries(NURingIntegratedController PRIVATE X11::X11 ${OpenCV_LIBS})

# Link the X11 library
#target_link_libraries(NURingIntegratedController PRIVATE ${X11_LIBRARIES})

# Link the OpenCV library
#target_link_libraries(NURingIntegratedController ${OpenCV_LIBS})
