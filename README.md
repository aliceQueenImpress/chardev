# Character Device Driver for a Fan Controller

## Overview

The objective of this project is to develop a Linux character device driver to control and monitor a smart system fan from Userland. The driver exposes a special file at `/dev/fan` and acts as the software bridge to manage the fan's physical behavior, hardware profiles, and safety constraints.

## Fan Specifications and Operational Modes

The driver manages a cooling fan that operates under specific constraints and modes:
* **Speed Regulation (0-100%)**: The fan speed is controlled via PWM (Pulse Width Modulation) duty cycles, bounded strictly between `0%` (stopped) and `100%` (maximum cooling).
* **Operational Modes**:
  * **Automatic Mode**: The system dynamically scales the fan speed based on thermal sensor inputs.
  * **Manual Mode**: The user overrides the system to force a fixed, constant speed.
* **Hardware Profiles**: The fan supports predefined cooling behaviors (e.g., `default`, `silent`, `performance`) to balance acoustic noise and thermal dissipation.

## Technical Objectives

The driver provides two distinct access interfaces to interact with the hardware:
1. **An application interface**: Allows a dedicated C application to precisely configure the fan's behavior (updating speed, toggling modes, switching profiles) or trigger a hardware factory `RESET` using binary data structures.

2. **A textual interface (`read`)**: Allows sysadmins or shell scripts to instantly retrieve a human-readable status report in plain text (ASCII) via standard terminal commands (e.g., `cat /dev/fan`).

The architecture must comply strictly with Linux kernel standards: it must be entirely **thread-safe**  and support **multi-opening** so that multiple applications can monitor or control the same hardware instance simultaneously without data corruption.

## How to launch 

### compile the test userspace application
```bash
gcc main.c -o test_fan
```

### load the driver in the kernel
```bash
sudo insmod chardev.ko
```

### Find the major number
```bash
dmesg | tail -n 5
# you will see line like this : "FAN driver(major: <major_num>) installed."
```

### create the node under the /dev/... with the major number
```bash
sudo mknod /dev/fan c <major_num> 0
sudo chmod 666 /dev/fan 
```

### launch the userspace application
```bash
./test_fan
```
