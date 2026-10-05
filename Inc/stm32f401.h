/*
 * stm32f401.h
 *
 *  Created on: 04-Oct-2026
 *      Author: Krishna
 */

#ifndef STM32F401_H_
#define STM32F401_H_

#include <stdint.h>

typedef struct
{
	volatile uint32_t MODER;
	volatile uint32_t OTYPER;
	volatile uint32_t OSPEEDR;
	volatile uint32_t PUPDR;
	volatile uint32_t IDR;
	volatile uint32_t ODR;
	volatile uint32_t BSRR;
	volatile uint32_t LCKR;
	volatile uint32_t AFRL;
	volatile uint32_t AFRH;
}GPIO_typedef;

typedef struct
{
	volatile uint32_t CR;
	volatile uint32_t PLLDFGR;
	volatile uint32_t CFGR;
	volatile uint32_t CIR;
	volatile uint32_t AHB1RSTR;
	volatile uint32_t AHB2RSTR;
	uint32_t reserved0[2];
	volatile uint32_t APB1RSTR;
	volatile uint32_t APB2RSTR;
	uint32_t reserved1[2];
	volatile uint32_t AHB1ENR;
	volatile uint32_t AHB2ENR;
	uint32_t reserved2[2];
	volatile uint32_t APB1ENR;
	volatile uint32_t APB2ENR;
	uint32_t reserved3[2];
	volatile uint32_t AHB1LPENR;
	volatile uint32_t AHB2LPENR;
	uint32_t reserved4[2];
	volatile uint32_t APB1LPENR;
	volatile uint32_t APB2LPENR;
	uint32_t reserved5[2];
	volatile uint32_t BDCR;
	volatile uint32_t CSR;
	uint32_t reserved6[2];
	volatile uint32_t SSCGR;
	volatile uint32_t PLLI2SCFGR;
	uint32_t reserved7[2];
	volatile uint32_t DCKCFGR;
}RCC_typedef;

typedef struct
{
	volatile uint32_t CR1;
	volatile uint32_t CR2;
	volatile uint32_t OAR1;
	volatile uint32_t OAR2;
	volatile uint32_t DR;
	volatile uint32_t SR1;
	volatile uint32_t SR2;
	volatile uint32_t CCR;
	volatile uint32_t TRISE;
	volatile uint32_t FLTR;
}I2C_typedef;

#define NVIC_ISER1 (*(volatile uint32_t *)0xE000E104U)

#define RCC    ((RCC_typedef    *)0x40023800U)

#define GPIOB  ((GPIO_typedef   *)0x40020400U)
#define GPIOC  ((GPIO_typedef   *)0x40020800U)

#define I2C1   ((I2C_typedef    *)0x40005400U)



#endif /* STM32F401_H_ */
