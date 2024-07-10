# keplerKV

> A key-value store.

## Prerequisites

CMake is required to build KeplerKV along with a C++ compiler compatible with the C++17 standard:

On Linux utilizing `apt`:
```bash
sudo apt-get install -y cmake
```

## Building

A `CMakeLists.txt` is located at the root of this repository. To build the `KeplerKV` binary:
```bash
mkdir build
cd build
cmake ..
make
```

## Usage

See the [manual](docs/manual.md) for a comprehensive list of commands and see the [examples](docs/examples.md) page for example usage.
