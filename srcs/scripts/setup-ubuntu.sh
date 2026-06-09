#!/usr/bin/env bash
set -euo pipefail

if ! command -v apt-get >/dev/null 2>&1; then
    echo "This setup script supports Ubuntu/Debian only."
    echo "Install Qt 6 Widgets, CMake, Ninja, and a C++ compiler for your platform."
    exit 1
fi

sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    ninja-build \
    qt6-base-dev

echo
echo "Dependencies installed. Run: make run"
