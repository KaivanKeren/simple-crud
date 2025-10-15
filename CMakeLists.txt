cmake_minimum_required(VERSION 3.10)

project(crud_data_mahasiswa_pg CXX)

set(CMAKE_CXX_STANDARD 17)

add_execu
table(crud_data_mahasiswa crud_data_mahasiswa.cpp)

# Pakai find_package dulu (lebih proper)
find_package(PostgreSQL)

if(PostgreSQL_FOUND)
    target_include_directories(crud_data_mahasiswa PRIVATE ${PostgreSQL_INCLUDE_DIRS})
    target_link_libraries(crud_data_mahasiswa PRIVATE ${PostgreSQL_LIBRARIES})
else()
    # fallback hardcode (biar gak error)
    target_include_directories(crud_data_mahasiswa PRIVATE /usr/include/postgresql)
    target_link_libraries(crud_data_mahasiswa PRIVATE pq)
endif()
