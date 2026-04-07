# Project Fary the Bot

Fary is an interactive AI-style companion bot for the ESP32 C3 Mini with an OLED display, buzzer, and button.

## Pinout Configuration (ESP32 C3 Mini)

| Component | Pin | Description |
|-----------|-----|-------------|
| **OLED SDA** | GPIO 20 | I2C Data |
| **OLED SCL** | GPIO 21 | I2C Clock |
| **Button** | GPIO 6 | Mode Selection (Short to GND) |

## Functionality

- **Boot Up**: Shows "HI! I AM FARY", "BY DRAC STUDIOS", and "POWERED BY ARPON" in large capital letters.
- **Single Press**: Cycle through 6 face expressions with a beep.
- **Double Click**: Changes to a random face expression with a random sound.
- **Triple Click**: Shows the "Cute/Shy" face (Default Mochi) and plays a shy sound.
