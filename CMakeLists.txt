cmake_minimum_required(VERSION 2.8)

project(robotpath)

add_library(howderek "libhowderek/howderek.c")
add_library(howderek_memory "libhowderek/howderek_memory.c")
add_library(howderek_array "libhowderek/howderek_array.c")
add_library(howderek_heap "libhowderek/howderek_heap.c")
add_library(howderek_skiplist "libhowderek/howderek_skiplist.c")
add_library(howderek_hashmap "libhowderek/howderek_hashmap.c")
add_library(howderek_kv "libhowderek/howderek_kv.c")
add_library(howderek_graph "libhowderek/howderek_graph.c")
add_library(howderek_grid "libhowderek/howderek_grid.c")

add_library(world "world.c")
add_library(robot "robot.c")
add_library(ui "ui.c")

add_executable("robotpath" "main.c")
add_executable("testUI" "testUI.c")
target_link_libraries("robotpath" howderek howderek_memory howderek_array howderek_heap howderek_skiplist howderek_hashmap howderek_kv howderek_graph howderek_grid world robot ui)
target_link_libraries("testUI" howderek howderek_memory howderek_array howderek_heap howderek_skiplist howderek_hashmap howderek_kv howderek_graph howderek_grid world robot ui)
