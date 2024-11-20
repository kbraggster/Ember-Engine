cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(EmberEngine)

# If using macOS, set system name to "macOS" instead of "Darwin"
Set(SystemName ${CMAKE_SYSTEM_NAME})
if (CMAKE_SYSTEM_NAME MATCHES "Darwin")
    Set(SystemName "macOS")
endif ()

# Set Output and Target directories
set(TargetDir "${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}-${SystemName}-${CMAKE_SYSTEM_PROCESSOR}")
set(ObjDir "${CMAKE_SOURCE_DIR}/bin-int/${CMAKE_BUILD_TYPE}-${SystemName}-${CMAKE_SYSTEM_PROCESSOR}")

# Set optimizations for Release build
# -fno-exceptions can be added as argument to strip exceptions
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -flto -march=native -fno-rtti")

# Set the directory containing your header files
set(SOURCE_DIR "Ember/src")
set(INCLUDE_DIR "Ember/src")

# Recursively find source and header files
file(GLOB_RECURSE SOURCE_FILES "${SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE HEADER_FILES "${INCLUDE_DIR}/*.h")

# Create executable
add_executable("${PROJECT_NAME}" ${SOURCE_FILES} ${HEADER_FILES})

# Set output directory
set(TargetDir ${TargetDir}/${PROJECT_NAME})
set(ObjDir ${ObjDir}/${PROJECT_NAME})
set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${TargetDir}"
        ARCHIVE_OUTPUT_DIRECTORY "${ObjDir}"
        LIBRARY_OUTPUT_DIRECTORY "${ObjDir}"
)

# Add precompiled header
target_precompile_headers(${PROJECT_NAME} PRIVATE Ember/src/empch.h)

# TODO: Platform specific macros
# Build Configurations
if (CMAKE_BUILD_TYPE MATCHES Debug)
    # Define the macro to enable debug breaks only for Debug build
    target_compile_definitions(${PROJECT_NAME} PRIVATE EM_ENABLE_ASSERTS)
    add_compile_definitions(EM_BUILD_TYPE_DEBUG)
endif ()
if (CMAKE_BUILD_TYPE MATCHES Release)
    add_compile_definitions(EM_BUILD_TYPE_RELEASE)
endif ()

# TODO: MoltenVK for macOS only
# Add libraries
find_package(Vulkan REQUIRED)
find_path(MVK_INCLUDE_DIR NAMES vulkan/vulkan.h PATH_SUFFIXES include)
find_library(MVK_LIBRARY NAMES MoltenVK PATH_SUFFIXES lib)

# Debugging information
message(STATUS "Vulkan_INCLUDE_DIRS: ${Vulkan_INCLUDE_DIRS}")
message(STATUS "Vulkan_LIBRARIES: ${Vulkan_LIBRARIES}")
message(STATUS "MoltenVK Include Dir: ${MVK_INCLUDE_DIR}")
message(STATUS "MoltenVK Library: ${MVK_LIBRARY}")

if (MVK_INCLUDE_DIR AND MVK_LIBRARY)
    # MoltenVK found
    message(STATUS "MoltenVK found")
    include_directories(${MVK_INCLUDE_DIR})
    target_link_libraries(${PROJECT_NAME}
            ${Vulkan_LIBRARIES}
            ${MVK_LIBRARY}
            "-framework IOSurface"
            "-framework Metal"
            "-framework Foundation"
            "-framework QuartzCore"
    )
else ()
    # MoltenVK not found
    message(FATAL_ERROR "MoltenVK not found. Please set MOLTENVK_SDK to the MoltenVK installation directory.")
endif ()

add_subdirectory(Ember/vendor/glfw)
add_subdirectory(Ember/vendor/spdlog)
#add_subdirectory(vendor/imgui)

# Set output directory for libraries
set_target_properties(glfw PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${ObjDir}
        LIBRARY_OUTPUT_DIRECTORY ${ObjDir})
set_target_properties(spdlog PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${ObjDir}
        LIBRARY_OUTPUT_DIRECTORY ${ObjDir})
#set_target_properties(imgui PROPERTIES
#        ARCHIVE_OUTPUT_DIRECTORY ${ObjDir}
#        LIBRARY_OUTPUT_DIRECTORY ${ObjDir})

# Include and link libraries
#target_include_directories(${PROJECT_NAME}
#        PRIVATE ${Vulkan_INCLUDE_DIRS}
#        PRIVATE vendor/glfw/include
#        PRIVATE vendor/spdlog/include
#        PRIVATE vendor/imgui)
target_include_directories(${PROJECT_NAME}
        PRIVATE ${Vulkan_INCLUDE_DIRS}
        PRIVATE Ember/vendor/glfw/include
        PRIVATE Ember/vendor/spdlog/include)

#target_link_libraries(${PROJECT_NAME} Vulkan::Vulkan glfw spdlog imgui glad)
target_link_libraries(${PROJECT_NAME} Vulkan::Vulkan glfw spdlog)