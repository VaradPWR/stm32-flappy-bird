# stm32-flappy-bird
Flappy Bird game implemented on STM32 using MAX7219 8x8 LED matrix display with SPI communication.

# Hardware Used
1) STM32 BluePill
2) MAX7219 Module (8x8 matrix)
3) Push button(jump)
4) Breadboard and Jumper wires.

## How It Works

- MAX7219 is interfaced with the STM32 using the **SPI peripheral**.
- Gap between pipes is **pseudo-randomly generated using `HAL_GetTick()`**
- Button input is **debounced in software** to avoid false triggers.
- A **framebuffer is used to render the entire frame** before updating the LED matrix, ensuring smooth animation.


# Gameplay
https://github.com/user-attachments/assets/7b28951e-3d49-4c21-af01-eb9d8fdb169d

# Possible Improvements
1) Difficulty Level
2) Sound Using Buzzer
