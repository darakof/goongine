# CMakeLists.txt
cmake_minimum_required(VERSION 3.10)
project(MyProject LANGUAGES CXX)

#-------------------------------------------------------------------------------
# 1. BUILD CONFIGURATION SETUP
#-------------------------------------------------------------------------------
# Set configurable build types with different optimization levels
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;Performance;Size" CACHE STRING "Build configurations" FORCE)

# Set default build type if not specified
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Define compiler flags for each build type
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Base flags for all build types
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

# Type-specific flags:
set(CMAKE_CXX_FLAGS_DEBUG          "-O0 -g -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE        "-O2 -DNDEBUG")
set(CMAKE_CXX_FLAGS_PERFORMANCE    "-O3 -march=native -DNDEBUG")
set(CMAKE_CXX_FLAGS_SIZE           "-Os -DNDEBUG")

#-------------------------------------------------------------------------------
# 2. PROJECT STRUCTURE SETUP
#-------------------------------------------------------------------------------
# Configure output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Create targets directory structure
file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
file(MAKE_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
file(MAKE_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

#-------------------------------------------------------------------------------
# 3. DEPENDENCY CONFIGURATION
#-------------------------------------------------------------------------------
# Find essential graphics libraries
find_package(OpenGL REQUIRED)

set(GLAD_ROOT "${CMAKE_SOURCE_DIR}/libs/glad/lib")
set(GLAD_INCLUDE_DIR "${GLAD_ROOT}/include")
set(GLAD_SOURCE "${GLAD_ROOT}/src")

set(GLFW_ROOT "${CMAKE_SOURCE_DIR}/libs/glfw")
set(GLFW_INCLUDE_DIR "${GLFW_ROOT}/lib/include")
set(GLFW_LIBRARY "${GLFW_ROOT}/lib/libglfw3.a")

# Find libdecor using pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBDECOR REQUIRED libdecor-0)

# Additional Wayland components
pkg_check_modules(WAYLAND REQUIRED
    wayland-client
    wayland-cursor
    wayland-egl
    xkbcommon
)

#-------------------------------------------------------------------------------
# 4. EXECUTABLE TARGET SETUP
#-------------------------------------------------------------------------------
# Gather source files
# Automatically gather all .cpp files recursively from src and any subfolders
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.cpp")

# Create main executable
add_executable(main ${SOURCES} ${GLAD_SOURCE})

# Configure include directories
target_include_directories(main PRIVATE
    include
    ${OPENGL_INCLUDE_DIR}
    ${GLAD_INCLUDE_DIR}
    ${GLFW_INCLUDE_DIR}
    ${LIBDECOR_INCLUDE_DIRS}
    ${WAYLAND_INCLUDE_DIRS}
)

# Link libraries
target_link_libraries(main
    glfw
    ${OPENGL_LIBRARIES}
    ${GLFW_LIBRARY}
    ${LIBDECOR_LIBRARIES}
    ${WAYLAND_LIBRARIES}
    dl
    pthread
)

# Optional X11 support
if(NOT WAYLAND_FOUND)
    find_package(X11 REQUIRED)
    target_link_libraries(main
        ${X11_LIBRARIES}
        Xrandr
        Xinerama
        Xi
        Xcursor
        Xxf86vm
    )
    message(STATUS "X11 support enabled (Wayland not available)")
endif()

#-------------------------------------------------------------------------------
# 5. ADVANCED RUN TARGETS WITH FULL CONFIGURATION
#-------------------------------------------------------------------------------
# Main run target
add_custom_target(run
    COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/main
    DEPENDS main
)

#run with debug statements
add_custom_target(run-debug
    COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/main -debug
    DEPENDS main
)

# Run with specific plugin
add_custom_target(run-with-plugin
    COMMAND ${CMAKE_COMMAND} -E echo "Running with specific plugin..."
    COMMAND ${CMAKE_COMMAND} -E echo "-----------------------------------------"
    COMMAND ${CMAKE_COMMAND} -E echo "Set PLUGIN=name and optionally PLUGIN_DIR=path"
    COMMAND ${CMAKE_COMMAND} -E echo "-----------------------------------------"
    COMMAND env XDG_SESSION_TYPE=wayland LD_LIBRARY_PATH="${DEFAULT_PLUGIN_DIR}:$ENV{LD_LIBRARY_PATH}" LIBDECOR_PLUGIN_DIR="${PLUGIN_DIR}" LIBDECOR_BACKEND="${PLUGIN}" ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/main
    DEPENDS main
    COMMENT "Running with specified plugin"
)

# Run with custom library
add_custom_target(run-with-lib
    COMMAND ${CMAKE_COMMAND} -E echo "Running with custom library..."
    COMMAND ${CMAKE_COMMAND} -E echo "-----------------------------------------"
    COMMAND ${CMAKE_COMMAND} -E echo "Set LIBNAME=library and optionally PLUGIN_DIR=path"
    COMMAND ${CMAKE_COMMAND} -E echo "-----------------------------------------"
    COMMAND env XDG_SESSION_TYPE=wayland LD_LIBRARY_PATH="${DEFAULT_PLUGIN_DIR}:$ENV{LD_LIBRARY_PATH}" LIBDECOR_PLUGIN_DIR="${PLUGIN_DIR}" ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/main
    DEPENDS main
    COMMENT "Running with specified library"
)

# Run with GTK plugin
add_custom_target(run-gtk
    COMMAND ${CMAKE_COMMAND} -E echo "Running with GTK decorations..."
    COMMAND env XDG_SESSION_TYPE=wayland LD_LIBRARY_PATH="${DEFAULT_PLUGIN_DIR}:$ENV{LD_LIBRARY_PATH}" LIBDECOR_PLUGIN_DIR="${DEFAULT_PLUGIN_DIR}" LIBDECOR_BACKEND=gtk ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/main
    DEPENDS main
    COMMENT "Running with GTK window decorations"
)

# Run with basic plugin
add_custom_target(run-basic
    COMMAND ${CMAKE_COMMAND} -E echo "Running with basic decorations..."
    COMMAND env XDG_SESSION_TYPE=wayland LIBDECOR_BACKEND=basic ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/main
    DEPENDS main
    COMMENT "Running with basic window decorations"
)

# X11 backend target
add_custom_target(run-x11
    COMMAND ${CMAKE_COMMAND} -E echo "Running with X11 backend..."
    COMMAND XDG_SESSION_TYPE=x11 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/main
    DEPENDS main
    COMMENT "Running application with X11 backend"
)

#-------------------------------------------------------------------------------
# 6. UTILITY TARGETS
#-------------------------------------------------------------------------------
# Dependency check
add_custom_target(check-deps
    COMMAND ${CMAKE_COMMAND} -E echo "Checking dependencies..."
    COMMAND pkg-config --exists glfw3 && echo "GLFW: installed" || echo "GLFW: missing (sudo apt install libglfw3-dev)"
    COMMAND pkg-config --exists libdecor-0 && echo "libdecor: installed" || echo "libdecor: missing (sudo apt install libdecor-0-dev)"
    COMMAND pkg-config --exists wayland-client && echo "Wayland: installed" || echo "Wayland: missing (sudo apt install libwayland-dev)"
    COMMAND bash -c '[ -d "${DEFAULT_PLUGIN_DIR}" ] && echo "Plugin directory exists" || echo "Plugin directory missing: ${DEFAULT_PLUGIN_DIR}" '
    COMMAND bash -c 'ls "${DEFAULT_PLUGIN_DIR}" 2>/dev/null | grep libdecor- || echo "No plugins found in ${DEFAULT_PLUGIN_DIR}" '
    COMMENT "Checking system dependencies"
)

# Configuration info target
add_custom_target(config-info
    COMMAND ${CMAKE_COMMAND} -E echo "Current Libdecor Configuration:"
    COMMAND ${CMAKE_COMMAND} -E echo "--------------------------------"
    COMMAND ${CMAKE_COMMAND} -E echo "Library name: ${DEFAULT_LIBDECOR_NAME}"
    COMMAND ${CMAKE_COMMAND} -E echo "Plugin directory: ${DEFAULT_PLUGIN_DIR}"
    COMMAND ${CMAKE_COMMAND} -E echo "Default plugin: ${DEFAULT_PLUGIN_NAME}"
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "Environment variables for runtime:"
    COMMAND ${CMAKE_COMMAND} -E echo "  LIBNAME      - Override libdecor library name"
    COMMAND ${CMAKE_COMMAND} -E echo "  PLUGIN_DIR   - Set plugin directory"
    COMMAND ${CMAKE_COMMAND} -E echo "  PLUGIN       - Select plugin to use"
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "Available plugins:"
    COMMAND ls "${DEFAULT_PLUGIN_DIR}" 2>/dev/null || echo "No plugins directory found"
    COMMENT "Showing current libdecor configuration"
)

#-------------------------------------------------------------------------------
# 7. RUNTIME CONFIGURATION GUIDE
#-------------------------------------------------------------------------------
# Display runtime configuration info
message("Predefined Targets:")
message("  run             : Full configuration (set all options)")
message("  run-with-plugin : Configure plugin and plugin directory")
message("  run-with-lib    : Configure library and plugin directory")
message("  run-gtk         : Use GTK decorations")
message("  run-basic       : Use basic decorations")
message("  run-x11         : Use X11 backend")
message("")
message("Examples:")
message("  make run PLUGIN=gtk PLUGIN_DIR=/usr/local/lib/plugins")
message("  make run-with-plugin PLUGIN=custom-backend")
message("  make run-with-lib LIBNAME=libdecor-experimental")
message("")
message("View configuration: make config-info")
message("Check dependencies: make check-deps")
message("================================================================")

#-------------------------------------------------------------------------------
# 8. BUILD TYPE DOCUMENTATION
#-------------------------------------------------------------------------------
# Help section explaining build types
message("\nBuild Type Options:")
message("  Debug:          -O0 -g (full debugging)")
message("  Release:        -O2 (balanced optimization)")
message("  Performance:    -O3 -march=native (max performance)")
message("  Size:           -Os (minimize binary size)")
message("Set with: cmake -DCMAKE_BUILD_TYPE=<type> ..")