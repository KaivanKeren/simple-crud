cmake_minimum_required(VERSION 3.10)

project(crud_pg CXX)

set(CMAKE_CXX_STANDARD 17)

add_executable(crud crud.cpp)

# Pakai find_package dulu (lebih proper)
find_package(PostgreSQL)

if(PostgreSQL_FOUND)
    target_include_directories(crud PRIVATE ${PostgreSQL_INCLUDE_DIRS})
    target_link_libraries(crud PRIVATE ${PostgreSQL_LIBRARIES})
else()
    # fallback hardcode (biar gak error)
    target_include_directories(crud PRIVATE /usr/include/postgresql)
    target_link_libraries(crud PRIVATE pq)
endif()
