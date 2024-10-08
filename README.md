# keplerKV

> A key-value store.

## Download

Each release comes with the compiled KeplerKV binary to download.

See the [latest release](https://github.com/sneha-afk/KeplerKV/releases)!

## Usage

See the [manual](docs/manual.md) for a comprehensive list of commands and see the [examples](docs/examples.md) page for example usage.

## Developers

Working on KeplerKV requires a C++ compiler that is compatible with the C++17 standard.

### Testing

A testing script is located in `tests/` that takes an input and corresponding output files located in `inputs/` and `outputs/` respectively. To run all tests:
```bash
cd tests
bash execute_all.sh
```

**Note: end any input file with `\q` as the program runs infinitely until this command is entered.**

### Building

Currently the project has two methods of building from scratch.

#### GNU Make

A `Makefile` is located at the root of the repository and is a quick way to build locally.

To install GNU Make with the `apt` package manger:
```bash
sudo apt-get install -y make
```

To build the `KeplerKV` binary from the root of the repository:
```bash
make
```

To clean your directory of object files and the binary:
```bash
make clean
```

#### CMake

A `CMakeLists.txt` is located at the root of the repository and is the preferred way to build locally.

To install CMake with the `apt` package manager:
```bash
sudo apt-get install -y cmake
```

To build the `KeplerKV` binary from the root of the repository:
```bash
mkdir build
cd build
cmake ..
make
```

