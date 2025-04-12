# Standard C++ HTTP Server

![C++](https://img.shields.io/badge/C++-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)

Standard C++ HTTP Server is a lightweight, multithreaded HTTP server built 
using minimal libraries.

## Table of Contents
- [Features](#features)
- [Platform Support](#platform-support)
- [Installation and Building](#installation-and-building)
  - [Prerequisites](#prerequisites)
  - [Building with CMake](#building-with-cmake)
- [Usage](#usage)
  - [Running the Server](#running-the-server)
  - [Endpoints](#endpoints)
- [Static Files](#static-files)
- [Dependencies](#dependencies)

## Features
- Listens on port 3000 by default.
- Serves static files from the `/public` directory.
- Efficient routing of HTTP requests.
- Multithreaded design for handling multiple connections.
- Automatic headers management for optimal browser support.

## Platform Support
- **Windows** (using WinSock)
- **Linux** (using POSIX sockets)

## Installation and Building

### Prerequisites
- A C++ 23 compliant compiler.
- CMake version 3.10 or later.
- For Windows targets, g++ version 14 or later.
- An Internet connection (needed to fetch dependencies).

### Building with CMake
```bash
mkdir build
cd build
cmake ..
cmake --build .
```

## Usage

### Running the Server
- **On Linux:**
  ```bash
  ./server [port] [publicDir]
  ```

- **On Windows:**
  ```bash
  server.exe [port] [publicDir]
  ```

### Endpoints
- **GET** `/hello`  
  Returns an HTML message with "Hello World!".

## Static Files
Place your static files inside the `/public` directory. The server automatically maps:
- Direct file requests (e.g., `/index.html`).
- Directory requests, if an `index.html` exists in a subdirectory.

## Dependencies
- C++ 23 Standard Library.
- [nlohmann/json](https://github.com/nlohmann/json) for JSON parsing.
- OpenSSL