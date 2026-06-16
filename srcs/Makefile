BUILD_DIR ?= /tmp/ackrail-build
RELEASE_BUILD_DIR ?= /tmp/ackrail-release
CMAKE ?= cmake
ARGS ?=
PERF_DATA ?= perf.data

.PHONY: all setup topology prepare-build-dir configure build run release-configure release-build run-release perf perf-report clean

all: build

setup:
	./scripts/setup-ubuntu.sh

topology:
	./scripts/import_idfm_gtfs.py --output map/paris_metro.json

prepare-build-dir:
	@if test -f "$(BUILD_DIR)/CMakeCache.txt"; then \
		cached_source="$$(sed -n 's|^CMAKE_HOME_DIRECTORY:INTERNAL=||p' "$(BUILD_DIR)/CMakeCache.txt")"; \
		cached_generator="$$(sed -n 's|^CMAKE_GENERATOR:INTERNAL=||p' "$(BUILD_DIR)/CMakeCache.txt")"; \
		if test "$$cached_source" != "$(CURDIR)" || test "$$cached_generator" != "Ninja"; then \
			echo "Removing incompatible CMake cache (source: $$cached_source, generator: $$cached_generator)"; \
			$(CMAKE) -E remove_directory "$(BUILD_DIR)"; \
		fi; \
	fi

configure: prepare-build-dir
	$(CMAKE) -S . -B $(BUILD_DIR) -G Ninja -DCMAKE_BUILD_TYPE=Debug

build: configure
	$(CMAKE) --build $(BUILD_DIR)

run: build
	$(BUILD_DIR)/ackrail $(ARGS)

release-configure:
	$(CMAKE) -S . -B $(RELEASE_BUILD_DIR) -G Ninja -DCMAKE_BUILD_TYPE=Release

release-build: release-configure
	$(CMAKE) --build $(RELEASE_BUILD_DIR)

run-release: release-build
	$(RELEASE_BUILD_DIR)/ackrail $(ARGS)

perf: release-build
	perf record -o $(PERF_DATA) -- $(RELEASE_BUILD_DIR)/ackrail $(ARGS)

perf-report:
	perf report -i $(PERF_DATA)

clean:
	$(CMAKE) -E remove_directory $(BUILD_DIR)
