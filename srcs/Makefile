BUILD_DIR := ../build
CMAKE ?= cmake
ARGS ?=

.PHONY: all setup topology configure build run clean

all: build

setup:
	./scripts/setup-ubuntu.sh

topology:
	./scripts/import_idfm_gtfs.py --output map/paris_metro.json

configure:
	$(CMAKE) -S . -B $(BUILD_DIR) -G Ninja -DCMAKE_BUILD_TYPE=Debug

build: configure
	$(CMAKE) --build $(BUILD_DIR)

run: build
	./$(BUILD_DIR)/ackrail $(ARGS)

clean:
	$(CMAKE) -E remove_directory $(BUILD_DIR)
