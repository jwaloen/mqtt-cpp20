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
* CMake 3.31+
* Git

### Building the Library and Tests

This project uses modern CMake. All dependencies (Eclipse Paho and GoogleTest) are downloaded automatically during the configuration step.

```bash
# 1. Clone the repository
git clone [https://github.com/YOUR_USERNAME/mqtt-cpp20.git](https://github.com/YOUR_USERNAME/mqtt-cpp20.git)
cd mqtt-cpp20

# 2. Configure the build
cmake -B build

# 3. Build the library and tests
cmake --build build

# 4. Run the test suite
cd build && ctest --output-on-failure
```

## 💻 Usage Example

Because the library utilizes the Pimpl idiom, including it in your project is incredibly clean. There are no C-style handles or manual teardown steps.

```cpp
#include "MQTTClientCpp.h"
#include <iostream>

int main() {
    try {
        // 1. Resource Acquisition Is Initialization (RAII)
        // Memory is safely allocated and configured in the constructor.
        mqtt::Client client("tcp://test.mosquitto.org:1883", "MyModernCppClient");

        // 2. Connect to the broker
        std::cout << "Connecting to broker...\n";
        client.connect();

        // 3. Publish a message
        std::string topic = "sensors/temperature";
        std::string payload = "23.5C";
        
        std::cout << "Publishing to " << topic << "...\n";
        client.publish(topic, payload);

        // 4. Disconnect gracefully
        client.disconnect();
        std::cout << "Success!\n";

    } catch (const std::exception& e) {
        std::cerr << "MQTT Error: " << e.what() << "\n";
        return 1;
    }

    // The destructor automatically cleans up the underlying C library handles,
    // even if an exception was thrown and disconnect() was skipped.
    return 0;
}
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
- [x] Implement Pimpl idiom for ABI stability
- [x] CI/CD test pipeline integration
- [x] Static Analysis
- [x] Connection Status Tracking
- [ ] Implement callbacks via `std::function` for topic subscriptions
- [ ] User options similar to C library
- [ ] Improved CMakeLists.txt (separate builds)
- [ ] README guide on how to include in projects
- [ ] Improve exceptions based on return code from C library
- [ ] Make pub/sub asynchronous
- [ ] Add TLS/SSL configuration options
