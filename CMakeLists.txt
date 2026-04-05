cmake_minimum_required(VERSION 3.31.6)

project(mqtt-cpp20)

include(FetchContent)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

set(GCC_CLANG_STRICT_FLAGS
    -Wall
    -Wextra
#    -Wpedantic # Warnings for non-ISO C++ code
#    -Werror # All warnings trigger error
    -Wshadow 
#    -Wconversion # Warnings on type conversion data loss
)

set(PAHO_BUILD_SAMPLES FALSE CACHE BOOL "Disable Paho samples" FORCE)
set(PAHO_BUILD_DOCUMENTATION FALSE CACHE BOOL "Disable Paho docs" FORCE)
set(PAHO_ENABLE_TESTING FALSE CACHE BOOL "Disable Paho tests" FORCE)

FetchContent_Declare(
    pahomqtt
    GIT_REPOSITORY https://github.com/eclipse-paho/paho.mqtt.c.git
    GIT_TAG        v1.3.16
)

FetchContent_MakeAvailable(pahomqtt)

# TESTS
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.17.0
)

FetchContent_MakeAvailable(googletest)
include(GoogleTest)

enable_testing()

add_executable(MQTTClient_test
    src/MQTTClientCpp.cpp    
    tests/main_test.cpp
    tests/MQTTClientCpp_test.cpp
)

target_include_directories(MQTTClient_test PRIVATE include)

target_link_libraries(MQTTClient_test PRIVATE 
    gmock_main
    paho-mqtt3c
)

target_compile_options(MQTTClient_test PRIVATE ${GCC_CLANG_STRICT_FLAGS})


gtest_discover_tests(MQTTClient_test) 
