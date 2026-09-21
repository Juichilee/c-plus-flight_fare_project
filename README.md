# Flight Fare Assistant

## Project Description
**Flight Fare Assistant** is a C++ library and CLI for computing optimal flight fares (direct or with one layover) between two airports. It validates airport codes and booking times, applies discounts for one-layover routes, and includes airport departure/arrival fees. The project is exercised entirely via unit tests using the Catch2 framework integrated with CMake.

## Dependencies
- Ubuntu (Linux)
- A C++17-capable compiler (e.g., `g++`, `clang++`)
- CMake ≥ 3.10
- Catch2 (header-only, included in `test/catch.hpp`)

## Directory Structure
README.md
flight_fare_assistant (root)
├── CMakeLists.txt
├── flight_fare_assistant.cpp
├── flight_fare_assistant.h
├── flight_fare_assistant_test.cpp
└── test
    ├── catch.hpp
    └── tests-main.cpp

## Setup
1. Make sure that the root project directory name is "flight_fare_assistant" otherwise, the build will fail.
2. In the root project directory, create and enter a build directory.
    ```bash
    mkdir build && cd build
    ```
    
3. From inside the "build/" directory, generate the build system with CMake:
    ```bash
    cmake ..
    ```

## How to Run
1. Still inside the "build/" directory:
    ```bash
    make
    ```
2. The unit tests from "flight_fare_assistant_test" should run and log the testing results.

    
