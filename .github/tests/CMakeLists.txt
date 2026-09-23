cmake_minimum_required(VERSION 3.16)
project(sheng_idle_rearm_test LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.6 REQUIRED COMPONENTS Gui)
find_package(KF6IdleTime 6.13 REQUIRED)

add_executable(sheng-idle-rearm-test sheng-idle-rearm-test.cpp)
target_link_libraries(sheng-idle-rearm-test PRIVATE Qt6::Gui KF6::IdleTime)
