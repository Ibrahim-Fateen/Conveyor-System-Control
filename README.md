# Smart Conveyor Monitoring & Control System

## Project Overview
This project simulates a smart conveyor system using an **ARM Cortex-M microcontroller** within the **Proteus** environment. The system is designed to:

- Measure conveyor speed  
- Control motor speed via a potentiometer  
- Detect passing objects  
- Display all relevant information on an LCD  
- Include a critical **emergency stop** function triggered by an external interrupt  
<img width="1311" height="914" alt="image" src="https://github.com/user-attachments/assets/db41de43-21df-405c-910e-a7fc8393497f" />

---
## Demo 
https://github.com/user-attachments/assets/c3c6bd98-c2a6-46be-8ff9-10d210853d14
---
## System Features & Components

### 1. Speed Measurement (Timer Capture)
- **Input:** Simulates pulse signals from an encoder using a Function Generator in Proteus  
- **Logic:** Measures the time between two rising edges using the microcontroller's Timer Input Capture mode  
- **Constraint:** Performed **without interrupts** (polling-based)  
- **Output:** Calculated speed displayed on the LCD  

### 2. Motor Speed Control (ADC + PWM)
- **Input:** Potentiometer connected to an ADC channel  
- **Logic:** Reads ADC value and generates a corresponding PWM signal to drive the motor  
- **Output:** Motor speed (0-100%) displayed on the LCD  

### 3. Object Detection (Polling)
- **Input:** Digital input simulating an IR sensor detecting objects  
- **Logic:** Software-based polling method to detect the falling edge  
- **Constraint:** Implemented purely in software, **no interrupts**  
- **Output:** Counter for passing objects updated on the LCD  

### 4. Emergency Stop (Interrupt)
- **Input:** Push button connected to an external interrupt pin  
- **Logic:** Uses an external interrupt to detect a button press  
- **Constraint:** Must be **interrupt-based**  
- **Output:** Motor stops immediately, and `"EMERGENCY STOP"` is shown on the LCD  

---

## Technical Stack
- **Simulation Environment:** Proteus  
- **Microcontroller:** ARM Cortex-M (STM32)  
- **Core Peripherals:**  
  - Timer (Input Capture Mode)  
  - ADC (Analog-to-Digital Converter)  
  - PWM (Pulse Width Modulation)  
  - GPIO (Digital input and LCD)  
  - EXTI (External Interrupts)  
- **Display:** 16x2 or 20x4 Character LCD  

---

## Contributors
- [Judy Ashmawy](https://github.com/JudyEssam)
- [Mostafa Ali](https://github.com/Mostafaali3)
- [Ibrahim Fateen](https://github.com/Ibrahim-Fateen)
- [Shahd Mahmoud](https://github.com/Shahd-A-Mahmoud)
