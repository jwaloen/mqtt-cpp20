# mqtt-cpp20

[![C++20](https://img.shields.io/badge/C++-20-blue.svg)](https://isocpp.org/)
[![CMake](https://img.shields.io/badge/CMake-3.31+-success.svg)](https://cmake.org/)
[![Test Status](https://img.shields.io/badge/tests-passing-brightgreen.svg)](#) A modern, memory-safe, and highly encapsulated C++20 wrapper around the popular [Eclipse Paho MQTT C Library](https://github.com/eclipse-paho/paho.mqtt.c). 

This project brings a highly robust and well tested legacy C MQTT library to modern C++ object-oriented design, providing a clean, safe, and intuitive interface for IoT and messaging applications.

## ✨ Key Engineering Features

As a portfolio project, this library was designed to enforce modern C++ best practices:

* **Strict Memory Safety (RAII):** Completely eliminates raw pointer management (`malloc`/`free`) and dangling handles. Underlying C resources are managed exclusively via `std::unique_ptr` with custom, non-throwing deleters to guarantee leak-free execution.
* **The Pimpl Idiom (Compilation Firewall):** The `mqtt::Client` utilizes the Pointer-to-Implementation (Pimpl) pattern. The underlying Paho C headers, structs, and macros are completely hidden from the user's inclusion path.
* **Modern CMake Architecture:** Uses `FetchContent` to seamlessly manage and link third-party dependencies (Paho C and GoogleTest) without requiring users to manually install them on their system.
* **Comprehensive Test Coverage:** Validated using `GoogleTest` to ensure strict state management (e.g., preventing publishing before connection) and robust error handling.

## 📦 Getting Started

### Prerequisites
* GCC with C++20 support
* CMake 3.14+
* Git

## Integration

This library requires **C++20** and **CMake 3.14+**. 

Because `mqtt-cpp20` uses modern target-based CMake, it automatically fetches and firewalls its own internal C dependencies (Eclipse Paho). You do not need to install Paho on your system to use this wrapper.

There are two recommended ways to include this library in your project:

### Option 1: CMake FetchContent (Recommended)
You can pull the library directly from GitHub during your CMake configuration step. Add the following to your project's `CMakeLists.txt`:

```cmake
include(FetchContent)

FetchContent_Declare(
    mqtt-cpp20
    GIT_REPOSITORY https://github.com/jwaloen/mqtt-cpp20.git
    GIT_TAG        main
)
FetchContent_MakeAvailable(mqtt-cpp20)

# Link it to your executable
add_executable(my_app main.cpp)
target_link_libraries(my_app PRIVATE mqtt-cpp20)
```

### Option 2: Git Submodule
If you prefer to vendor dependencies, you can add this repository as a Git submodule.

1. Add the submodule to your project:
   ```bash
   git submodule add https://github.com/jwaloen/mqtt-cpp20.git third_party/mqtt-cpp20
   ```
2. Add it to your `CMakeLists.txt`:
   ```cmake
   add_subdirectory(third_party/mqtt-cpp20)

   add_executable(my_app main.cpp)
   target_link_libraries(my_app PRIVATE mqtt-cpp20)
   ```

### Quick Start Example
Once linked, simply include the header and instantiate the client:

```cpp
#include "MQTTClientCpp.h"
#include <iostream>

int main() {
    try {
        mqtt::Client client("tcp://test.mosquitto.org:1883", "MyUniqueClientID");
        client.connect();
        
        client.publish("MyTopic/Telemetry", "System Online");
        std::cout << "Message published successfully.\n";
        
        client.disconnect();
    } catch (const std::exception& e) {
        std::cerr << "MQTT Error: " << e.what() << '\n';
    }
    return 0;
}
```

## Local Development

If you are developing this library or running the test suite locally, you need a C++20 compatible compiler and CMake 3.14+.

### 1. Clone the Repository
```bash
git clone https://github.com/YOUR_GITHUB_USERNAME/mqtt-cpp20.git
cd mqtt-cpp20
```

### 2. Build and Run Tests (Debug Mode)
To compile the library with debug symbols and run the Google Test suite, you must explicitly enable the `BUILD_TESTING` flag.

```bash
# Configure the build directory for testing
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON

# Compile the library and tests (-j uses multiple CPU cores for speed)
cmake --build build -j 4

# Execute the test suite
cd build
ctest --output-on-failure
```

### 3. Build for Production (Release Mode)
To verify the library compiles perfectly for a production environment without the testing frameworks polluting the build, disable the testing flag.

```bash
# Configure a separate build directory for release
cmake -B build_release -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF

# Compile the highly-optimized static library
cmake --build build_release -j 4
```

## 🏗️ Project Structure

```text
├── CMakeLists.txt        # Modern CMake configuration
├── include/
│   └── MQTTClientCpp.h   # Clean, public C++20 API (No C headers)
├── src/
│   └── MQTTClientCpp.cpp # Hidden Pimpl implementation & custom destructor
└── tests/
    ├── main_test.cpp          # GoogleTest entry point
    └── MQTTClientCpp_test.cpp # Unit tests for memory safety and states
```

## 🚀 Future Roadmap

- [x] Modern CMake project implementation
- [x] Establish memory-safe C++ wrapper base
- [x] Implement Pimpl idiom for encapsulation of C code
- [x] CI/CD test pipeline integration
- [x] Static Analysis
- [x] Connection Status Tracking
- [x] Implement callbacks via `std::function` for topic subscriptions
- [x] Force initializations to avoid "bad" states (data loss)
- [x] Implement trampoline function for disconnect
- [x] Implement trampoline fundtion for delivery complete
- [ ] User options similar to C library
- [x] Improved CMakeLists.txt (separate builds)
- [x] README guide on how to include in projects
- [x] Improve exceptions based on return code from C library
- [ ] Test against own MQTT server, not test.mosquitto.org
- [ ] Make pub/sub asynchronous
- [ ] Add TLS/SSL configuration options

## Important Learning experiences
- Pointer to Implementation is how I avoid including the C library headers in my header file, which would break the encapsulation of the C code.
- Static trampoline function can act as a C function when sent as a callback funtion.
