# STM32F407 Bare-Metal UART (Interrupt-Based TX/RX)

This project demonstrates a **bare-metal UART driver** for the **STM32F407** using **direct register access** and **interrupts**.

No HAL.  
No CubeMX.  
Only the reference manual and hardware registers.

---

## ✨ Features

- USART2 full-duplex (TX + RX)
- Interrupt-based UART (RXNE & TXE)
- Echo characters typed in terminal
- ENTER → moves to new line
- BACKSPACE → deletes last character
- Simple terminal-like behavior
- Default clock (HSI 16 MHz)

---

## 🧠 What You Learn

- STM32 memory-mapped registers
- GPIO alternate function setup
- USART baud-rate calculation
- NVIC interrupt enabling
- RXNE and TXE interrupt handling
- Terminal input handling (CR/LF, backspace)

---

## 🔌 Hardware Connections

| STM32F407 | USB-UART |
|---------|----------|
| PA2 (TX) | RX |
| PA3 (RX) | TX |
| GND | GND |

⚠️ TX/RX must be crossed  
⚠️ GND is mandatory

---

## ⚙️ UART Configuration

| Parameter | Value |
|--------|------|
| USART | USART2 |
| Baud rate | 9600 |
| Clock | HSI 16 MHz (default) |
| Data bits | 8 |
| Parity | None |
| Stop bits | 1 |

---

## 🖥️ Terminal Setup

### picocom (Linux)

```bash
picocom -b 9600 --flow none /dev/ttyUSB0

