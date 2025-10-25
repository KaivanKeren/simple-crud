cmake_minimum_required(VERSION 3.10)

project(crud_data_mahasiswa_pg LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add executable
add_executable(crud_data_mahasiswa crud_data_mahasiswa.cpp)

# Try to find PostgreSQL
find_package(PostgreSQL REQUIRED)

if(PostgreSQL_FOUND)
    message(STATUS "PostgreSQL found: ${PostgreSQL_VERSION_STRING}")
    target_include_directories(crud_data_mahasiswa PRIVATE ${PostgreSQL_INCLUDE_DIRS})
    target_link_libraries(crud_data_mahasiswa PRIVATE ${PostgreSQL_LIBRARIES})
else()
    message(WARNING "PostgreSQL not found! Using manual include/link path.")
    target_include_directories(crud_data_mahasiswa PRIVATE /usr/include/postgresql)
    target_link_libraries(crud_data_mahasiswa PRIVATE pq)
endif()