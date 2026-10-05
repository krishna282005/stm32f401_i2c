# STM32F401 I²C Driver

Bare-metal, register-level I²C driver developed for the **STM32F401CCUx** microcontroller using direct peripheral register access.

The driver was implemented without STM32 HAL or high-level driver libraries and was developed by following the STM32F401 reference manual and validating the peripheral behavior on actual hardware.

---

## Overview

The driver implements both **Controller** and **Target** I²C operation using polling/blocking techniques.

### Implemented Modes

- Controller Transmitter
- Controller Receiver
- Target Transmitter
- Target Receiver

The implementation directly controls the STM32F401 I²C peripheral through its registers and status flags.

---

## Features

### Controller Mode

- START condition generation
- 7-bit target addressing
- Controller transmission
- Controller reception
- ACK/NACK handling
- STOP condition generation
- Single-byte reception
- Two-byte reception
- Multi-byte reception
- TXE handling
- RXNE handling
- BTF handling
- ADDR handling

### Target Mode

- Own 7-bit target address configuration
- Address matching
- Target Receiver
- Target Transmitter
- Multi-byte reception
- Multi-byte transmission
- STOP detection
- NACK / Acknowledge Failure handling

### Error Handling

The driver includes handling for:

- Acknowledge Failure (AF)
- Bus Error (BERR)
- Arbitration Lost (ARLO)
- Invalid transfer length

---

# Hardware

## Microcontroller

**STM32F401CCUx**

## I²C Peripheral

**I²C1**

| Signal | STM32 Pin |
|--------|-----------|
| SCL | PB6 |
| SDA | PB7 |

The I²C pins are configured for:

- Alternate Function mode
- Open-drain output
- I²C1 alternate function
- Pull-up configuration

---

# I²C Configuration

The current implementation uses:

| Parameter | Value |
|-----------|-------|
| APB1 clock | 16 MHz |
| I²C speed | 100 kHz |
| I²C mode | Standard Mode |
| Addressing | 7-bit |
| Own target address | `0x42` |
| Duty cycle | Standard / default |

The important timing registers are configured as:

```c
I2C1->CR2   = 16U;
I2C1->CCR   = 80U;
I2C1->TRISE = 17U;
