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

# Add subdirectories
add_subdirectory(Ember)
add_subdirectory(Editor)