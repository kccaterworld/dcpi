# DefCon Badge Peripheral

An attempt to turn a Defcon 32 Badge into a peripheral for my headless Raspberry Pi 3B.
Probably a Rust daemon on the Pi and C firmware (Pico SDK)

## Features

System Monitor: CPU and Memory Stats (temp, speed, uptime)

Process Manager: See and terminate processes, resource consumption

Networking: Network scanning, connecting, disconnecting

SSH: Status, enable, disable

Systemd: Status, enable, disable, list

Files: Browse and read files

Power: Restart, shutdown

Dashboards: Custom configurable screens

LED Control: Controllable from other programs, fully programmable

Logs: System and service logs


## Plan

### Phase 1 — Hardware foundation

Establish communication, initialize the touchscreen, initialize the display, and control the LEDs.

### Phase 2 — Pi daemon

Implement the communication protocol, system monitoring, configuration loading, and local API.

### Phase 3 — Dynamic interface

Implement the widget renderer and Pi-defined dashboard layouts.

### Phase 4 — Management functionality

Add Wi-Fi, SSH, processes, services, filesystem browsing, and power controls.

### Phase 5 — LED automation

Implement effects, event triggers, priorities, and the local programming API.

### Phase 6 — Reliability

Add reconnect handling, configuration validation, permission controls, logging, and automatic daemon startup.

### Fancies

Custom scripting language for custom screens, animations, and widgets