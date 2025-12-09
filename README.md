# cell-sim-toy

**Educational** repository that simulates a very small “cell tower” and “UE (phone)” using TCP/IP only — **no radio**, **no cellular protocols**, **no interaction with real mobile networks**.

**NOTE / SAFETY**: This project is strictly for learning how control-plane messages might be handled in software. Do **not** attempt to deploy anything on real cellular frequencies or implement LTE/5G stacks that interact with carriers. The ESP32 example uses Wi-Fi TCP only.

## Contents
- `tower/` — C server (tower simulator)
- `phone_pc/` — C client (phone simulator)
- `esp32/` — Arduino sketch for ESP32 to simulate a UE over Wi-Fi/TCP
- `asm_examples/` — x86_64 NASM toy program that parses commands from stdin

## Quick start (Linux / macOS)
1. Build tower and phone:
   ```bash
   make all


repo that simulates small cell tower & UE (phone) using TCP/IP only, no radio, no cellular protocols, no interaction /w real mobile net
- tower - C server
- phone_pc - C client
- esp32 - arduino sketch from esp32 simulating UE over wifi/tcp
- asm - x86 nasm toy parses command from stdin
