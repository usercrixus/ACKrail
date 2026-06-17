BUILD_DIR ?= /tmp/ackrail-build
RELEASE_BUILD_DIR ?= /tmp/ackrail-release
CMAKE ?= cmake
ARGS ?=
PERF_DATA ?= perf.data
PERF_PARANOID ?= /proc/sys/kernel/perf_event_paranoid
PERF_PARANOID_VALUE ?= 0

.PHONY: all setup topology prepare-build-dir configure build run release-configure release-build run-release perf-setup perf perf-report clean

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

perf-setup:
	@if test -r "$(PERF_PARANOID)"; then \
		paranoid="$$(cat "$(PERF_PARANOID)")"; \
		if test "$$paranoid" -ne "$(PERF_PARANOID_VALUE)"; then \
			echo "Setting kernel.perf_event_paranoid=$(PERF_PARANOID_VALUE) for this boot."; \
			sudo sysctl kernel.perf_event_paranoid="$(PERF_PARANOID_VALUE)"; \
		fi; \
	fi

perf: perf-setup release-build
	@tmp_data="$(PERF_DATA).tmp"; \
	trap 'rm -f "$$tmp_data"' EXIT INT TERM; \
	perf record -o "$$tmp_data" -- $(RELEASE_BUILD_DIR)/ackrail $(ARGS); \
	mv "$$tmp_data" "$(PERF_DATA)"

perf-report:
	perf report -i $(PERF_DATA)

clean:
	$(CMAKE) -E remove_directory $(BUILD_DIR)
