# KeplerKV
**A lightweight, high-performance NoSQL key-value store built in C++.**

KeplerKV is designed for efficient data management, supporting a versatile data model and an extensible query language. It’s perfect for applications requiring fast read/write operations and flexible data storage.

## Features
- 🕊️ Lightweight: minimal dependencies past standard libraries, low footprint performance
- 💪 Versatile data model: supports integers, floats, strings, lists, matrices, and references.
- ✍️ Extensible query language: simple syntax for ease in data manipulation. *"KeplerQL"* has a ring to it.
- 🔬 Transactions and ACID: features transaction support and ensures data consistency

## Download
Pre-compiled binaries are available for each release.

> [📦 Latest Release][1]

## Usage
Run the KeplerKV binary to start the key-value store:
```bash
./KeplerKV
```

For a comprehensive list of commands and examples, check out the [Manual][2].

## For Developers
### Building from Source
KeplerKV is built on the C++11 standard.

Building from scratch can be done with either CMake (recommended) or GNU Make.

---

#### CMake (recommended)
To install CMake with the apt package manager:
```bash
sudo apt-get install -y cmake
```

To build into a directory called `build/`
```bash
mkdir build && cd build
cmake ..
make
```

#### GNU Make
To install GNU Make with the apt package manger:
```bash
sudo apt-get install -y make
```

A `Makefile` is located at the root of the repository and is a quick way to build locally. To build:
```bash
make
```

To clean up files:
```bash
make clean
```

---

### Testing
A testing script is located in `tests/` that takes an input and corresponding output files located in `inputs/` and `outputs/` respectively. To run all tests:

```bash
cd tests
bash execute_all.sh
```

## License
KeplerKV is open-source software licensed under the MIT License.


[1]: https://github.com/sneha-afk/KeplerKV/releases
[2]: https://github.com/sneha-afk/KeplerKV/wiki/Manual
