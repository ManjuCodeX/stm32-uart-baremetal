# STM32F407 Bare-Metal UART (TX / RX Echo)

This repository contains a **bare-metal UART implementation** for the **STM32F407** microcontroller using **direct register access**.  
No HAL, no CubeMX, no CMSIS drivers — only the **reference manual and registers**.

The project demonstrates **full-duplex UART communication** where data typed in a terminal is **received by the MCU and echoed back**.

---

## ✨ Features

- Bare-metal (no HAL / CubeMX)
- USART2 full-duplex (TX + RX)
- Polling mode (blocking)
- UART echo example
- Simple and beginner-friendly
- Tested with USB-to-UART + picocom

---

## 🧠 What You Will Learn

- STM32 memory-mapped registers
- RCC clock enabling
- GPIO alternate function configuration
- USART baud-rate calculation
- TXE and RXNE flag handling
- Full-duplex UART communication

---

## 🧩 Hardware Requirements

- STM32F407 board (any variant exposing PA2 / PA3)
- USB-to-UART adapter (CP2102 / CH340 / FTDI)
- Jumper wires

---

## 🔌 Hardware Connections

| STM32 Pin | USB-UART Pin |
|---------|-------------|
| PA2 (TX) | RX |
| PA3 (RX) | TX |
| GND | GND |

⚠️ TX and RX must be crossed.  
⚠️ Common GND is mandatory.

---

## ⚙️ UART Configuration

| Parameter | Value |
|--------|------|
| USART | USART2 |
| Baud rate | 9600 |
| Clock source | HSI (16 MHz default) |
| Data bits | 8 |
| Parity | None |
| Stop bits | 1 |
| Flow control | None |

---

## 🧪 How It Works

1. MCU initializes GPIO and USART2
2. USART runs at **9600 baud** using default **16 MHz HSI**
3. Terminal sends characters
4. MCU receives data using `RXNE` flag
5. MCU sends data back using `TXE` flag
6. Terminal displays echoed characters

---

## 🖥️ Terminal Setup

### Using picocom (Linux)


picocom -b 9600 --flow none /dev/ttyUSB0

