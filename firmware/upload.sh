#!/usr/bin/env bash
# Kompilasi lalu upload ke ESP32 asli, kemudian buka serial monitor.
# Pakai: ./upload.sh [port]   (default /dev/ttyUSB0; cek dengan: arduino-cli board list)
set -euo pipefail
cd "$(dirname "$0")"
PORT="${1:-/dev/ttyUSB0}"
FQBN=esp32:esp32:esp32
arduino-cli compile --fqbn "$FQBN" --build-property "compiler.cpp.extra_flags=-DSIMULASI=0" sensor-banjir
arduino-cli upload --fqbn "$FQBN" -p "$PORT" sensor-banjir
arduino-cli monitor -p "$PORT" -c baudrate=115200
