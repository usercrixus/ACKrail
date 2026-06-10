# Development

The simulator uses C++20, Qt 6 Widgets, Boost Graph, and CMake. The map is rendered
directly in C++ with `QPainter`. The `Makefile` is a small convenience wrapper
around CMake.

## Ubuntu / Debian

Enter the source directory, install the dependencies, and run the application:

```bash
cd srcs
make setup
make run
```

The setup command installs Qt from the operating system packages. Qt is not
committed to this repository.

The other useful commands are:

```bash
make          # configure and build
make run      # build and launch
make clean    # remove generated files
```

Build files default to `/tmp/ackrail-build` because CMake build trees can be
unreliable on VirtualBox shared folders. Override the location when needed:

```bash
make BUILD_DIR=$HOME/.cache/ackrail/build
```

The embedded Paris and Île-de-France Métro + RER topology is loaded by
default. It contains all Métro services and RER A-E from the official
Île-de-France Mobilités GTFS feed. To load another topology JSON file:

```bash
make run ARGS=map/paris_metro.json
```

Topology files contain geographic nodes and links:

```json
{
  "nodes": [
    { "id": 1, "name": "Station A", "latitude": 48.86, "longitude": 2.34 },
    { "id": 2, "name": "Station B", "latitude": 48.85, "longitude": 2.35 }
  ],
  "links": [
    { "id": 1, "from": 1, "to": 2, "line": "1", "color": "#FFCD00" }
  ]
}
```

Regenerate the full Métro and RER topology from the current official
Île-de-France Mobilités GTFS feed:

```bash
make topology
```

The generated file represents scheduled station-to-station service topology.
GTFS does not describe every physical track, switch, siding, tunnel, or depot.

## VS Code

Install these extensions:

- `C/C++` by Microsoft
- `CMake Tools` by Microsoft
- `Qt C++` by Qt Group

Open the repository, select `srcs/CMakeLists.txt` as the CMake source
directory, and run `make setup` once from `srcs/` in the integrated terminal.
Then use `make run`. CMake Tools can also configure and run the same project.

## Other Platforms

Install Qt 6.4 or newer, CMake, Ninja, and a C++20 compiler. Then build from
the repository root:

```bash
cmake -S srcs -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

The source and build configuration are portable. Dependency installation and
final application packaging remain platform-specific.
