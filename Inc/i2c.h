/*
 * i2c.h
 *
 *  Created on: 04-Oct-2026
 *      Author: krishna
 */

#ifndef I2C_H_
#define I2C_H_

#include <stdint.h>

#define I2C_OK       0
#define I2C_ERR_BERR 1
#define I2C_ERR_ARLO 2
#define I2C_ERR_AF   3

void I2C1_init(void);
uint8_t I2C1_start(void);
void I2C1_stop(void);
void led_on(void);
void led_off(void);

uint8_t I2C1_send_address(uint8_t address);
uint8_t I2C1_write_byte(uint8_t data);
uint8_t I2C1_write(uint8_t address,uint8_t data);
uint8_t I2C1_read_byte(uint8_t address, uint8_t *data);
uint8_t I2C1_read2_byte(uint8_t address, uint8_t *data1, uint8_t *data2);
uint8_t I2C1_read(uint8_t address, uint8_t *data, uint32_t length);


uint8_t I2C1_target_transmitter(uint8_t *data, uint32_t length);
uint8_t I2C1_target_receiver (uint8_t *data, uint32_t length);

#endif /* I2C_H_ */
