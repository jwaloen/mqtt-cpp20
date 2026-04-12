cmake_minimum_required(VERSION 3.31.6)

project(mqtt-cpp20 VERSION 0.5.0 LANGUAGES CXX C)

include(FetchContent)
include(CTest)

# ------------------------------------------------------------------------------
# 1. COMPILER SETTINGS
# ------------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(GCC_CLANG_STRICT_FLAGS
    -Wall
    -Wextra
    -Werror
    -Wshadow 
)

# ------------------------------------------------------------------------------
# 2. PAHO DEPENDENCY
# ------------------------------------------------------------------------------
set(PAHO_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
set(PAHO_BUILD_DOCUMENTATION OFF CACHE BOOL "" FORCE)
set(PAHO_BUILD_SHARED OFF CACHE BOOL "" FORCE)
set(PAHO_BUILD_STATIC ON CACHE BOOL "" FORCE) 
set(PAHO_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
set(PAHO_WITH_SSL OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    eclipse-paho-mqtt-c
    GIT_REPOSITORY https://github.com/eclipse-paho/paho.mqtt.c.git
    GIT_TAG        v1.3.16
)
FetchContent_MakeAvailable(eclipse-paho-mqtt-c)

# ------------------------------------------------------------------------------
# 3. PRODUCTION TARGET
# ------------------------------------------------------------------------------
add_library(mqtt-cpp20 STATIC src/MQTTClientCpp.cpp)

target_compile_options(mqtt-cpp20 PRIVATE ${GCC_CLANG_STRICT_FLAGS})

target_include_directories(mqtt-cpp20 PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

target_link_libraries(mqtt-cpp20 PRIVATE paho-mqtt3c-static)

# ------------------------------------------------------------------------------
# 4. TESTING SUITE
# ------------------------------------------------------------------------------

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
    
    # Prevent GTest from installing itself if the user runs 'make install'
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        v1.14.0
    )
    FetchContent_MakeAvailable(googletest)
    
    add_executable(MQTTClient_test tests/MQTTClientCpp_test.cpp)

    target_link_libraries(MQTTClient_test PRIVATE 
        mqtt-cpp20
        gtest_main
        gmock_main
    )

    include(GoogleTest)
    gtest_discover_tests(MQTTClient_test) 

endif()