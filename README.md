# TetsunNeko_v2.0

An autonomous Mini Sumo robot control system built for Arduino. This firmware features an advanced multi-sensor array using an **Adafruit VL53L0X Time-of-Flight (ToF)** laser distance sensor for tracking opponents, a **Sharp IR proximity sensor** for flank coverage, and a **3-channel analog line sensor** array for ring edge detection.

The robot is engineered around a strict prioritization loop: Safety Switch Check ➔ Edge Avoidance ➔ Frontal Attack Tracking ➔ Flank Acquisition ➔ Active Searching.



---

## 🏆 Competition History

This robot competed in the **BattleLab Robotica 2026** competition, participating in the highly competitive **3kg Autonomous Sumo Robot** category. 

### Team Members
* **Mutiu Alexandra Maria**
* **Kaminschi Karina Maria** 
* **Marita-Thuri Maria** -

The robot was designed, built, and calibrated from scratch to meet the strict 3kg weight limit and dimensional requirements, successfully demonstrating real-time sensor processing.

## 🛠️ Hardware Setup

### Pin Mapping

| Component | Code Identifier | Arduino Pin | Type | Connection Notes |
| :--- | :--- | :--- | :--- | :--- |
| **Start/Stop Switch** | `START_STOP_PIN` | `22` | Digital Input | Pull-down / Safety remote module receiver |
| **Sharp IR Sensor** | `SHARP_PIN` | `A3` | Analog Input | Side-looking proximity radar |
| **Floor Sensor (Left)** | `LIGHT_L` | `A0` | Analog Input | QRE1113 or TCRT5000 line tracker |
| **Floor Sensor (Center)**| `LIGHT_C` | `A1` | Analog Input | QRE1113 or TCRT5000 line tracker |
| **Floor Sensor (Right)** | `LIGHT_R` | `A2` | Analog Input | QRE1113 or TCRT5000 line tracker |
| **Left Motor PWM** | `ENA` | `5` | PWM Output | L298N / Dual H-Bridge Enable A |
| **Right Motor PWM** | `ENB` | `6` | PWM Output | L298N / Dual H-Bridge Enable B |
| **Left Motor Direction** | `IN1`, `IN2` | `8`, `9` | Digital Output | H-Bridge Motor 1 Direction Logic |
| **Right Motor Direction**| `IN3`, `IN4` | `10`, `11` | Digital Output | H-Bridge Motor 2 Direction Logic |
| **ToF Sensor** | `SDA` / `SCL` | `A4` / `A5` | I2C | Target acquisition (Default I2C Pins) |

---

## 🤖 Control Logic & Priority Hierarchy

The robot evaluates conditions in a nested priority structure every loop cycle:

1. **Safety Cutoff (Absolute Priority):** If `START_STOP_PIN` reads `LOW`, all motors immediately cut power.
2. **Edge Intercept:** If any floor sensor exceeds `LIGHT_LIMIT` (500), it takes immediate precedence over combat tracking, pushing the robot forward/away from the white border.
3. **Frontal Charge:** If the ToF sensor reports a target between `50mm` and `1200mm`, the robot overrides searching maneuvers and charges straight ahead.
4. **Flank Tracking:** If the front is clear but the Left Sharp IR reads above `SHARP_LIMIT` (300), the robot performs a fast left pivot to align the target with its front plow.
5. **Active Scan:** When no enemies are visible, the robot spins in place to the right to map the dohyo.

---

## 📦 Software Dependencies

To compile and upload this project, ensure you have the following libraries installed in your Arduino IDE environment:

* **`Wire.h`** (Standard built-in Arduino I2C communication library)
* **`Adafruit_VL53L0X`** by Adafruit (Available via **Tools** ➔ **Manage Libraries...**)

---

## ⚙️ Calibration Variables

Key operational thresholds can be calibrated directly within the header defines of the primary `.ino` sketch:

```cpp
#define TOF_LIMIT_CLOSE 700  // Distance threshold for close quarters combat (mm)
#define TOF_LIMIT_FAR   1200 // Maximum tracking threshold for opponent detection (mm)
#define SHARP_LIMIT     300  // Flank IR analog reading threshold for target acquisition
#define LIGHT_LIMIT     500  // Ring boundary white line analog detection threshold
