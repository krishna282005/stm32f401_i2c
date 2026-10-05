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
```

---

# I²C Timing Calculations

These calculations are kept in the README so that the I²C timing configuration can be reconstructed in the future without having to derive the values again.

---

## 1. APB1 Clock

I²C1 is connected to the **APB1 bus**.

Current configuration:

```text
PCLK1 = 16 MHz
```

Therefore, the APB1 clock period is:

```text
T_PCLK1 = 1 / f_PCLK1

        = 1 / 16 MHz

        = 62.5 ns
```

So:

```text
T_PCLK1 = 62.5 ns
```

---

## 2. CR2.FREQ Calculation

The `CR2.FREQ` field represents the APB1 peripheral clock frequency in MHz.

Since:

```text
PCLK1 = 16 MHz
```

the value written to `CR2.FREQ` is:

```text
CR2.FREQ = 16
```

Therefore:

```c
I2C1->CR2 = 16U;
```

### Result

```text
CR2.FREQ = 16
```

---

## 3. CCR Calculation

The required I²C clock frequency is:

```text
f_SCL = 100 kHz
```

Therefore, the SCL clock period is:

```text
T_SCL = 1 / f_SCL

      = 1 / 100 kHz

      = 10 µs
```

For **Standard Mode I²C** with the normal duty cycle:

```text
T_SCL = 2 × CCR × T_PCLK1
```

Rearranging:

```text
CCR = T_SCL / (2 × T_PCLK1)
```

We already calculated:

```text
T_SCL   = 10 µs
T_PCLK1 = 62.5 ns
```

Convert the APB1 period to microseconds:

```text
62.5 ns = 0.0625 µs
```

Substituting:

```text
CCR = 10 µs / (2 × 0.0625 µs)

    = 10 / 0.125

    = 80
```

Therefore:

```c
I2C1->CCR = 80U;
```

### Final CCR Value

```text
PCLK1 = 16 MHz
I²C   = 100 kHz
CCR   = 80
```

---

## 4. TRISE Calculation

For **Standard Mode I²C**, the maximum allowed SCL rise time is:

```text
t_RISE(max) = 1000 ns
```

The STM32F401 TRISE calculation is:

```text
TRISE = t_RISE(max) / T_PCLK1 + 1
```

We already calculated:

```text
T_PCLK1 = 62.5 ns
```

Substituting:

```text
TRISE = 1000 ns / 62.5 ns + 1

      = 16 + 1

      = 17
```

Therefore:

```c
I2C1->TRISE = 17U;
```

### Final TRISE Value

```text
PCLK1       = 16 MHz
T_PCLK1     = 62.5 ns
t_RISE(max) = 1000 ns
TRISE       = 17
```

---

## 5. Final Timing Configuration

The complete timing configuration is therefore:

```c
I2C1->CR2   = 16U;
I2C1->CCR   = 80U;
I2C1->TRISE = 17U;
```

Which corresponds to:

```text
┌─────────────────────────────────┐
│ APB1 Clock       = 16 MHz       │
│ APB1 Period      = 62.5 ns      │
│                                 │
│ I²C Speed        = 100 kHz      │
│ SCL Period       = 10 µs        │
│                                 │
│ CR2.FREQ         = 16           │
│ CCR              = 80           │
│ TRISE            = 17           │
└─────────────────────────────────┘
```

### Quick Reference

```text
Given:

PCLK1 = 16 MHz
f_SCL = 100 kHz
Standard Mode
t_RISE(max) = 1000 ns

Calculate:

T_PCLK1 = 1 / 16 MHz
        = 62.5 ns

T_SCL   = 1 / 100 kHz
        = 10 µs

CCR     = T_SCL / (2 × T_PCLK1)
        = 10 µs / (2 × 0.0625 µs)
        = 80

TRISE   = t_RISE(max) / T_PCLK1 + 1
        = 1000 ns / 62.5 ns + 1
        = 17

Therefore:

CR2.FREQ = 16
CCR      = 80
TRISE    = 17
```

---

# Project Structure

```text
stm32f401_i2c/
│
├── Inc/
│   ├── i2c.h
│   └── stm32f401.h
│
├── Src/
│   ├── i2c.c
│   ├── main.c
│   ├── syscalls.c
│   └── sysmem.c
│
├── Startup/
│   └── startup_stm32f401ccux.s
│
├── .settings/
│   ├── language.settings.xml
│   └── org.eclipse.core.resources.prefs
│
├── .cproject
├── .project
├── .gitignore
└── STM32F401CCUX_FLASH.ld
```

---

# Driver Architecture

The implementation follows the actual I²C peripheral state machine.

```text
                         STM32F401 I²C1
                              │
                ┌─────────────┴─────────────┐
                │                           │
          Controller Mode              Target Mode
                │                           │
          ┌─────┴─────┐               ┌─────┴─────┐
          │           │               │           │
         TX          RX              RX          TX
          │           │               │           │
        Write        Read           Receive     Transmit
```

The software synchronizes with the I²C peripheral using status flags and register operations.

Important status flags include:

```text
SB      START condition generated
ADDR    Address matched / address phase completed
TXE     Data register empty
RXNE    Receive data available
BTF     Byte transfer finished
AF      Acknowledge failure
STOPF   STOP condition detected
BERR    Bus error
ARLO    Arbitration lost
```

---

# Controller Transmitter

The Controller Transmitter generates a START condition, sends the target address with the write direction, transmits the requested bytes and finally generates STOP.

```text
START
  ↓
Target Address + Write
  ↓
ADDR
  ↓
Data → DR
  ↓
TXE / BTF
  ↓
Next Data
  ↓
STOP
```

Example:

```c
uint8_t data = 0x55;

I2C1_write(0x42, &data, 1);
```

The implementation also checks for transmission-related errors such as:

- Acknowledge Failure
- Bus Error
- Arbitration Lost

---

# Controller Receiver

The Controller Receiver supports:

- 1-byte reception
- 2-byte reception
- N-byte reception

The STM32F401 I²C peripheral requires different ACK, POS, BTF and STOP handling depending on the number of bytes being received.

---

## Single-Byte Reception

```text
START
  ↓
Address + Read
  ↓
ADDR
  ↓
ACK disabled
  ↓
STOP requested
  ↓
RXNE
  ↓
Read DR
```

---

## Two-Byte Reception

```text
START
  ↓
Address + Read
  ↓
ADDR
  ↓
ACK disabled
  ↓
POS enabled
  ↓
BTF
  ↓
STOP
  ↓
Read two bytes
```

---

## N-Byte Reception

For transfers greater than two bytes, the implementation uses the required STM32F401 I²C receive sequence involving:

- RXNE
- BTF
- ACK
- STOP

The final bytes are handled separately according to the STM32F401 I²C peripheral receive sequence.

---

# Target Receiver

The STM32F401 can operate as an I²C target using its configured own address.

Current target address:

```text
0x42
```

The target receiver follows this general sequence:

```text
START
  ↓
Address Match
  ↓
ADDR
  ↓
Clear ADDR
  ↓
RXNE
  ↓
Read DR
  ↓
Next Byte
  ↓
STOPF
```

Received bytes are stored into the supplied data buffer.

The STOP condition is detected using `STOPF` and cleared using the required status-register/control-register sequence.

---

# Target Transmitter

The Target Transmitter responds when the Controller requests data from target address `0x42`.

The controller sends:

```text
Target Address + Read
```

The STM32F401 then transmits the requested data.

```text
START
  ↓
Address + Read
  ↓
ADDR
  ↓
Clear ADDR
  ↓
Load DR
  ↓
TXE
  ↓
Load next byte
  ↓
NACK / STOP
```

The target transmitter uses `TXE` to determine when the next data byte can be loaded into the data register.

An `AF` event represents the controller's NACK and marks the end of a normal target-transmit sequence.

---

# Error Handling

The driver defines return codes for common I²C conditions:

```c
#define I2C_OK          0
#define I2C_ERR_BERR    1
#define I2C_ERR_ARLO    2
#define I2C_ERR_AF      3
#define I2C_ERR_INVALID 4
```

These allow the application to determine whether a transaction completed successfully or encountered an error.

---

# Hardware Validation

The driver was tested on actual STM32F401 hardware using an Arduino as the opposite I²C device.

Both controller and target operation were tested.

---

## Controller Validation

Validated:

- Controller transmission
- Single-byte reception
- Two-byte reception
- Multi-byte reception
- Address handling
- ACK/NACK behavior
- TXE handling
- RXNE handling
- BTF handling

Example multi-byte reception:

```text
Arduino → STM32F401

0x11 0x22 0x33 0x44 0x55
```

The STM32F401 successfully received the complete sequence.

---

## Target Validation

Validated:

- Target address detection
- Target reception
- Target transmission
- Multi-byte transfers
- Consecutive Target RX → TX transactions
- NACK / AF behavior

Example validated transaction:

```text
Arduino → STM32F401

0x11 0x22 0x33 0x44 0x55

STM32F401 → Arduino

0x11 0x22 0x33 0x44 0x55
```

The target receiver successfully received the data and the target transmitter subsequently returned the same data.

---

# Validation Status

| Feature | Status |
|---|---|
| I²C Initialization | ✅ |
| Controller Transmitter | ✅ |
| Controller Receiver | ✅ |
| 1-byte Controller RX | ✅ |
| 2-byte Controller RX | ✅ |
| N-byte Controller RX | ✅ |
| Target Receiver | ✅ |
| Target Transmitter | ✅ |
| ACK/NACK Handling | ✅ |
| ADDR Handling | ✅ |
| TXE Handling | ✅ |
| RXNE Handling | ✅ |
| BTF Handling | ✅ |
| STOPF Handling | ✅ |
| AF Handling | ✅ |
| BERR Handling | Implemented |
| ARLO Handling | Implemented |
| BERR Hardware Test | ⏸ Not performed |
| ARLO Hardware Test | ⏸ Not performed |
| Polling/Blocking Driver | ✅ |
| Interrupt-driven Driver | 🚧 Next Phase |

> **Note:** BERR and ARLO handling are implemented in the driver, but dedicated hardware fault-injection tests were not performed.

---

# Development Approach

The driver was developed directly from the STM32F401 peripheral documentation rather than using STM32 HAL APIs.

The learning and implementation approach was:

```text
Physical I²C Bus
       ↓
Electrical Behavior
       ↓
I²C Peripheral Hardware
       ↓
Peripheral Registers
       ↓
Status Flags
       ↓
Driver Code
       ↓
Actual Hardware Test
```

The goal was to understand the relationship between the physical I²C bus, the STM32 peripheral state machine, register operations and software behavior.

---

# Polling / Blocking Architecture

The current implementation is intentionally polling/blocking.

The CPU waits for the relevant peripheral event before continuing.

For example:

```c
while (!(I2C1->SR1 & TXE))
{
}
```

or:

```c
while (!(I2C1->SR1 & RXNE))
{
}
```

This makes the peripheral state machine explicit and was used as the foundation before moving to interrupt-driven operation.

---

# Next Phase

The next stage of this project is to move from polling/blocking I²C communication to an **interrupt-driven I²C driver**.

Planned areas include:

- I²C event interrupts
- I²C error interrupts
- NVIC configuration
- Interrupt-driven Controller TX
- Interrupt-driven Controller RX
- Interrupt-driven Target TX
- Interrupt-driven Target RX
- I²C transaction state management
- Event/error handling
- Callback-based driver architecture

---

# References

The implementation was developed using the following primary references:

- **STM32F401xB/C and STM32F401xD/E Reference Manual — RM0368**
- **STM32F401xB/C Datasheet — DS9716**
- **ARM Cortex-M4 Programming Manual — PM0214**

---

# Author

**Krishna Patel**

B.Tech – Electronics & Communication Engineering  
Nirma University

**Focus:** Embedded Systems • Bare-Metal C • STM32 • I²C • UART • RTOS
