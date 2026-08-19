# SNNS v4.3

The Stuttgart Neural Network Simulator (SNNS) is a simulation environment
for training and running neural networks, developed at the University of
Stuttgart and later the University of Tübingen.

This repository contains the original SNNS v4.3 source, a [CMake](https://cmake.org/)
build system, and a Python interface to the kernel.

## Repository layout

| Path              | Contents |
|-------------------|----------|
| `kernel/`         | The SNNS simulator kernel |
| `tools/`          | Command-line tools built on the kernel (`snns2c`, `batchman`, `analyze`, ...) |
| `xgui/`           | The X11/Xt/Xaw graphical user interface |
| `contrib/enzo/`   | Genetic algorithm tool ENZO |
| `contrib/nessus/` | The Nessus network compiler |
| `contrib/vistra/` | The Vistra visualization tool |
| `python/`         | Python interface to the kernel (extension module + examples) |
| `examples/`       | Example SNNS network, pattern, and configuration files |
| `man/`            | Man pages |
| `cmake/`          | CMake helper modules used by the build |
| `default.cfg`     | Default SNNS window configuration file |
| `help.hdoc`       | English help documentation for context-sensitive help within SNNS; also usable as a plain-text manual |

Additional documentation:

- `Readme.install` - the *original* autotools-based (`configure && make`)
  build instructions, kept for historical reference. The CMake build
  described below has superseded it.
- `Readme.license` - the SNNS license
- `Readme.bugs` - a list of known problems
- `Readme.linux` - Linux-specific notes

## Building with CMake

### Requirements

- CMake >= 3.5 (>= 3.18 if enabling the Python interface, see below)
- A C compiler
- `bison` and `flex`
- X11, Xt, Xaw3d development libraries (only if building the GUI)
- Python 3 development headers (only if enabling the Python interface)

### Configure

```sh
mkdir build && cd build
cmake ..
```

Useful options (pass as `-DOPTION=ON/OFF` to `cmake`):

| Option | Default | Description |
|--------|---------|--------------|
| `SNNS_ENABLE_XGUI` | `ON` | Build the X11 GUI (requires X11/Xaw3d) |
| `SNNS_ENABLE_ENZO` | `OFF` | Build the ENZO genetic algorithm tool |
| `SNNS_ENABLE_NESSUS` | `OFF` | Build the Nessus network compiler |
| `SNNS_ENABLE_VISTRA` | `OFF` | Build the Vistra visualization tool |
| `SNNS_ENABLE_PYTHON_INTERFACE` | `OFF` | Build the Python interface to the kernel |
| `COIN_COMPILE_WARNINGS` | `OFF` | Enable an extra set of compiler warnings |
| `COIN_COMPILE_COVERAGE` | `OFF` | Build with code coverage instrumentation (Debug builds only) |
| `COIN_COMPILE_STATIC` | `OFF` | Link statically |

For example, to build everything including the GUI, ENZO, and the
Python interface:

```sh
cmake -DSNNS_ENABLE_ENZO=ON -DSNNS_ENABLE_PYTHON_INTERFACE=ON ..
```

To build a headless setup (kernel + command-line tools only, no X11
dependency):

```sh
cmake -DSNNS_ENABLE_XGUI=OFF ..
```

### Build

```sh
cmake --build . -j
```

### Install

```sh
cmake --install .
```

Install locations follow [GNUInstallDirs](https://cmake.org/cmake/help/latest/module/GNUInstallDirs.html)
conventions relative to `CMAKE_INSTALL_PREFIX` (default: a self-contained
`snns_binary` directory inside the build directory):

- Executables: `${CMAKE_INSTALL_BINDIR}` (`bin`)
- Man pages: `${CMAKE_INSTALL_MANDIR}/man1`
- Examples, docs, and default config: `${CMAKE_INSTALL_DATADIR}/SNNS/{examples,doc,conf}`
  (`share/SNNS/...`)
- Python interface examples: `${CMAKE_INSTALL_DATADIR}/SNNS/python/examples`

The Python interface itself (the `krui` extension module and the `snns`
package) is installed separately, straight into the Python interpreter's
own site-packages directory, so that `import snns` works right away with
no `PYTHONPATH` needed.
