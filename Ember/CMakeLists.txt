cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(Ember)

# Set the directory containing your header files
set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Recursively find source and header files
file(GLOB_RECURSE SOURCE_FILES "${SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE HEADER_FILES "${INCLUDE_DIR}/*.h")

# Create static library
add_library("${PROJECT_NAME}" STATIC ${SOURCE_FILES} ${HEADER_FILES})

# Vulkan library path
set(VULKAN_LIBRARY_DIR $ENV{VULKAN_SDK}/lib)

# Use shaderc library from Vulkan path
set(SHADERC_LIB "${VULKAN_LIBRARY_DIR}/libshaderc_shared.dylib")
# Required by shaderc
set(CMAKE_INSTALL_PREFIX vendor/shaderc/third_party/abseil_cpp)

target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_DIR})

# Set output directory
set(TargetDir ${TargetDir}/${PROJECT_NAME})
set(ObjDir ${ObjDir}/${PROJECT_NAME})
set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${TargetDir}"
        ARCHIVE_OUTPUT_DIRECTORY "${ObjDir}"
        LIBRARY_OUTPUT_DIRECTORY "${ObjDir}"
)

# Add precompiled header
target_precompile_headers(${PROJECT_NAME} PRIVATE src/empch.h)

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

add_subdirectory(vendor/glfw)
add_subdirectory(vendor/spdlog)
add_subdirectory(vendor/imgui)
add_subdirectory(vendor/shaderc)

# Set output directory for libraries
set_target_properties(glfw PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${ObjDir}
        LIBRARY_OUTPUT_DIRECTORY ${ObjDir})
set_target_properties(spdlog PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${ObjDir}
        LIBRARY_OUTPUT_DIRECTORY ${ObjDir})
set_target_properties(imgui PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${ObjDir}
        LIBRARY_OUTPUT_DIRECTORY ${ObjDir})

# Include and link libraries
target_include_directories(${PROJECT_NAME}
        PRIVATE ${Vulkan_INCLUDE_DIRS}
        PRIVATE Ember/vendor/glfw/include
        PRIVATE Ember/vendor/spdlog/include
        PRIVATE Ember/vendor/imgui
        PRIVATE Ember/vendor/shaderc/libshaderc/include)


target_link_libraries(${PROJECT_NAME}
        Vulkan::Vulkan
        glfw
        spdlog
        imgui
        ${SHADERC_LIB})