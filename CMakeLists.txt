cmake_minimum_required(VERSION 3.23)

project(x6100_gui)

include_directories(.)
add_subdirectory(lvgl)
add_subdirectory(lv_drivers)
add_subdirectory(src)
