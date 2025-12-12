# Remote-Controlled Gesture Robot — Work in Progress

This project implements a **wireless gesture-controlled robot**, where the user controls the robot's movement using a glove equipped with an ESP32 and an accelerometer. Communication between the glove and the robot is achieved using **ESP-NOW**, enabling low-latency, peer-to-peer data transmission without WiFi pairing.

The robot itself uses another ESP32, an L298N dual H-bridge driver module, and two DC motors to achieve differential steering. Future improvements include replacing the L298N with custom-built H-bridge circuits using MOSFETs for increased efficiency and PWM-based motor speed control.

---

# System Architecture

```
 ┌───────────────────────────────────────────┐
 │               CONTROL GLOVE               │
 │  - ESP32                                  │
 │  - Accelerometer (gesture detection)       │
 │  - ESP-NOW packet transmitter              │
 └───────────────────────┬───────────────────┘
                         │ ESP-NOW
                         ▼
 ┌───────────────────────────────────────────┐
 │                 ROBOT UNIT                │
 │  - ESP32                                  │
 │  - Motor driver (L298N → future MOSFET)   │
 │  - Dual DC motors (differential steering) │
 └───────────────────────────────────────────┘
```

---

# Gesture-Controlled Glove

The glove acts as a **wireless remote controller**.

### Hardware

* ESP32 development board
* 3-axis accelerometer (e.g., MPU6050 / ADXL345)
* Battery + charging module

### Functionality

1. Reads accelerometer data at fixed intervals
2. Interprets hand tilt / movement as control commands:

   * Tilt forward → Drive forward
   * Tilt backward → Reverse
   * Tilt left → Turn left
   * Tilt right → Turn right
3. Sends compact command packets via ESP-NOW to the robot

### ESP-NOW Advantages

* Very low latency
* No WiFi network required
* Direct peer-to-peer communication
* Reliable for remote control applications

---

# Robot Unit

The robot receives commands and drives motors accordingly.

### Hardware

* ESP32
* L298N dual H-bridge motor driver (current implementation)
* Two DC motors for differential steering
* Battery pack
* Chassis (2WD)

### Control Flow

1. ESP32 receives gesture packets via ESP-NOW
2. Decodes movement commands
3. Controls the L298N outputs to:

   * Move forward/backward (both motors same direction)
   * Turn left/right (motors opposite directions or PWM mixing)

---

# Planned Hardware Upgrade

The L298N module will be replaced with **custom H-bridge circuits**.

### Why Replace the L298N?

* It wastes energy (voltage drop up to 2V per channel)
* It runs hot
* It cannot provide smooth PWM at high frequencies

### Planned Improvements

* Dual MOSFET H-bridge design
* Low Rds(on) MOSFETs for cooler and more efficient operation
* Gate driver circuits for reliable switching
* True PWM speed control

### Expected Benefits

* Higher motor torque
* Lower power consumption
* More precise control (acceleration curves, braking, etc.)
* Extendable to 4WD or tracked robots

---

# Communication Protocol

The glove sends small binary or JSON packets via ESP-NOW.

### Example Packet

```json
{
  "f": 0.8,   // Forward/backward intensity (-1 to +1)
  "t": -0.4    // Turn intensity (-1 to +1)
}
```

The robot applies differential steering based on these values:

```
left_motor_speed  = f - t
right_motor_speed = f + t
```

---

# Software Overview

### Glove Firmware

* Reads accelerometer data
* Computes orientation vectors
* Filters noise (moving average / low-pass)
* Generates control values
* Sends packet via ESP-NOW

### Robot Firmware

* Initializes ESP-NOW receiver
* Parses command packet
* Maps values to motor speeds
* Controls motors through L298N (digital + PWM)

---

# 🚧 Work in Progress

Planned improvements:

* Fully custom H-bridge using MOSFETs
* Adding PWM-based speed control
* Implement robot status telemetry back to glove
* Add PID stabilization for smoother movement
* Battery monitoring & failsafe cutoff
* 3D-printed glove and robot housings

---

# Getting Started

### 1. Flash Glove Firmware

* Connect accelerometer
* Test orientation readings via serial
* Pair ESP-NOW MAC addresses

### 2. Flash Robot Firmware

* Set correct MAC of glove unit
* Test L298N motor control
* Validate command reception

### 3. Test & Tune

* Adjust thresholds for gesture interpretation
* Tune ESP-NOW packet rate
* Adjust PWM parameters for smooth driving

---

# License

This project is currently experimental and under active development. All documentation and code are intended for educational and research purposes.

If you'd like, I can generate circuit diagrams, pinout tables, ESP-NOW packet format documentation, or a MOSFET H-bridge schematic for the future upgrade.
