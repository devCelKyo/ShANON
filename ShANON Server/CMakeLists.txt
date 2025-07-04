cmake_minimum_required(VERSION 3.10)  # Minimum CMake version
project(App)                        # Project name

if (WIN32)
    add_compile_definitions(PLATFORM_WINDOWS=1)
elseif (APPLE)
    add_compile_definitions(PLATFORM_APPLE=2)
endif()
# define apple macro, adapt code, compile again

# Add all source files to the executable
add_executable(app
    main.c
    clients.c
)

# Include current directory for headers
target_include_directories(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})