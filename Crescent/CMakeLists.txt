cmake_minimum_required(VERSION 3.16)
project(Crescent)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/$<CONFIG>-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}/${PROJECT_NAME})

file(GLOB_RECURSE SANDBOX_SOURCE_FILES "src/*.cpp")

add_executable(${PROJECT_NAME} ${SANDBOX_SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/Luna/src
)

target_link_libraries(${PROJECT_NAME} PRIVATE Luna)

add_custom_target(${PROJECT_NAME}_copy_assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/Assets/"
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Assets/"
    COMMENT "Copying Assets"
)
add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}_copy_assets)
