BUILD_DIR ?= /tmp/ackrail-build
CMAKE ?= cmake
ARGS ?=

.PHONY: all setup topology prepare-build-dir configure build run clean

all: build

setup:
	./scripts/setup-ubuntu.sh

topology:
	./scripts/import_idfm_gtfs.py --output map/paris_metro.json

prepare-build-dir:
	@if test -f "$(BUILD_DIR)/CMakeCache.txt"; then \
		cached_source="$$(sed -n 's|^CMAKE_HOME_DIRECTORY:INTERNAL=||p' "$(BUILD_DIR)/CMakeCache.txt")"; \
		if test "$$cached_source" != "$(CURDIR)"; then \
			echo "Removing CMake cache created for $$cached_source"; \
			$(CMAKE) -E remove_directory "$(BUILD_DIR)"; \
		fi; \
	fi

configure: prepare-build-dir
	$(CMAKE) -S . -B $(BUILD_DIR) -G Ninja -DCMAKE_BUILD_TYPE=Debug

build: configure
	$(CMAKE) --build $(BUILD_DIR)

run: build
	$(BUILD_DIR)/ackrail $(ARGS)

clean:
	$(CMAKE) -E remove_directory $(BUILD_DIR)
