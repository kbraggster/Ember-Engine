cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 20)

project(Editor)

# Recursively find source files
file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable("${PROJECT_NAME}" ${SOURCES})

# Set executable name
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "EmberEngine")

# Set output directory
set(TargetDir ${TargetDir}/${PROJECT_NAME})
set(ObjDir ${ObjDir}/${PROJECT_NAME})
set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${TargetDir}
        ARCHIVE_OUTPUT_DIRECTORY ${ObjDir}
        LIBRARY_OUTPUT_DIRECTORY ${ObjDir}
)

# Add Ember library
target_link_libraries(${PROJECT_NAME} Ember)

