#include <stdint.h>
#include "stm32f401.h"

#define PE    (1U << 0)
#define START (1U << 8)
#define STOP  (1U << 9)
#define ACK   (1U << 10)
#define POS   (1U << 11)

#define SB    (1U << 0)
#define ADDR  (1U << 1)
#define BTF   (1U << 2)
#define STOPF (1U << 4)
#define RXNE  (1U << 6)
#define TXE   (1U << 7)
#define BERR  (1U << 8)
#define ARLO  (1U << 9)
#define AF    (1U << 10)

#define BUSY (1U << 1)

#define I2C_OK       	0
#define I2C_ERR_BERR 	1
#define I2C_ERR_ARLO 	2
#define I2C_ERR_AF      3
#define I2C_ERR_INVALID 4

void led_on(void);
void led_off(void);

void I2C1_init(void)
{
	RCC->AHB1ENR |=  (1U << 2); //enable GPIOC
	GPIOC->MODER &= ~(3U << (13*2)); // clean for led
	GPIOC->MODER |=  (1U << (13*2)); // set as output



	RCC->APB1ENR |= (1U << 21); //ENABLE clock for i2c1 peripheral
	RCC->AHB1ENR |= (1U << 1);  //ENABLE clock for GPIOB peripheral

	GPIOB->MODER &= ~(3U << 2*6); //clear PB6
	GPIOB->MODER &= ~(3U << 2*7); //clear PB7
	GPIOB->MODER |= (2U << 2*6); //set as alternate function
	GPIOB->MODER |= (2U << 2*7); //set as alternate function

	GPIOB->OTYPER |= (1U << 6)|(1U << 7); //set both lines as open-drain
	// Internal pull-up on PB6 and PB7
	GPIOB->PUPDR &= ~((3U << 12) | (3U << 14));
	GPIOB->PUPDR |=  ((1U << 12) | (1U << 14));

	GPIOB->AFRL &= ~(15U << 4*6); //clear
	GPIOB->AFRL &= ~(15U << 4*7); //clear
	GPIOB->AFRL |= (4U << 4*6); //PB6 as SCL
	GPIOB->AFRL |= (4U << 4*7); //PB7 as SDA

	I2C1->CR1 &= ~PE; // PE=0
	I2C1->CR1 = 0;

	// i2c SCL timing and clock configuration
	//APB1 clk = 16MHz
	I2C1->CR2   = 16U;
	//Standard mode SCL = 100KHz
	I2C1->CCR   = 80U;
	//Maximum SCL rise time = 1000ns
	I2C1->TRISE = 17U;

	//Own TARGET address = 0x42
	I2C1->OAR1 = (1U << 14) | (0x42U << 1);

	I2C1->CR1 |= PE; // PE=1
	I2C1->CR1 |= ACK;
}

uint8_t I2C1_start(void)
{
	I2C1->CR1 |= START; // START bit = 1
	while (1) //Wait for SB
	{
		uint8_t sr1 = I2C1->SR1;
		if (sr1 & BERR)
		{
			I2C1->CR1 |= STOP;
			I2C1->SR1 &= ~BERR;
			return I2C_ERR_BERR;
		}
		if (sr1 & ARLO)
		{
			I2C1->SR1 &= ~ARLO;
			return I2C_ERR_ARLO;
		}
		if (sr1 & SB)
		{
			break;
		}
	}
	return I2C_OK;
}

uint8_t I2C1_send_address(uint8_t address)
{
	// 7 bit address + WRITE mode ............. so STM32 is as Controller Transmitter
	I2C1->DR = (address << 1)|0;

	while(1)// wait for ADDR, it will set when ACK will be received
	{
		uint32_t sr1 = I2C1->SR1;
		if (sr1 & BERR)
		{
			I2C1->CR1 |= STOP;
			I2C1->SR1 &= ~BERR;
			return I2C_ERR_BERR;
		}
		if (sr1 & AF)
		{
			I2C1->CR1 |= STOP;
			I2C1->SR1 &= ~AF;
			return I2C_ERR_AF;
		}
		if (sr1 & ARLO)
		{
			I2C1->SR1 &= ~ARLO;
			return I2C_ERR_ARLO;
		}
		if (sr1 & ADDR)
		{
			// clear ADDR
			(void)I2C1->SR1;
			(void)I2C1->SR2;
			return I2C_OK;
		}
	}

}

uint8_t I2C1_write_byte(uint8_t data)
{
	while(1) //wait for DR to get empty
	{
		uint32_t sr1 = I2C1->SR1;
		if (sr1 & BERR)
		{
			I2C1->CR1 |= STOP;
			I2C1->SR1 &= ~BERR;
			return I2C_ERR_BERR;
		}
		if (sr1 & AF)
		{
			I2C1->CR1 |= STOP;
			I2C1->SR1 &= ~AF;
			return I2C_ERR_AF;
		}
		if (sr1 & ARLO)
		{
			I2C1->SR1 &= ~ARLO;
			return I2C_ERR_ARLO;
		}
		if (sr1 & TXE)
		{
			break;
		}
	}

	I2C1->DR = data;

	while(1) //wait for complete byte transfer
	{
		uint32_t sr1 = I2C1->SR1;
		if (sr1 & BERR)
		{
			I2C1->CR1 |= STOP;
			I2C1->SR1 &= ~BERR;
			return I2C_ERR_BERR;
		}
		if (sr1 & AF)
		{
			I2C1->CR1 |= STOP;
			I2C1->SR1 &= ~AF;
			return I2C_ERR_AF;
		}
		if (sr1 & ARLO)
		{
			I2C1->SR1 &= ~ARLO;
			return I2C_ERR_ARLO;
		}
		if (sr1 & BTF)
		{
			break;
		}
	}
	return I2C_OK;
}

void I2C1_stop(void)
{
	I2C1->CR1 |= STOP; // stop bit = 1
}

uint8_t I2C1_write(uint8_t address, uint8_t data)
{
	uint8_t status = I2C1_start();
	if (status != I2C_OK)
		return status;

	status = I2C1_send_address(address);
	if (status != I2C_OK)
		return status;

	status = I2C1_write_byte(data);
	if (status != I2C_OK)
		return status;

	I2C1_stop();

	return I2C_OK;
}

uint8_t I2C1_read_byte(uint8_t address, uint8_t *data)
{
	uint8_t status = I2C1_start();
	if (status != I2C_OK)
		return status;

	I2C1->DR = (address << 1)|1;

	while(1)// wait for ADDR, it will set when ACK will be received
	{
		uint32_t sr1 = I2C1->SR1;
		if (sr1 & BERR)
		{
			I2C1->CR1 |= STOP;
			I2C1->SR1 &= ~BERR;
			return I2C_ERR_BERR;
		}
		if (sr1 & AF)
		{
			I2C1->CR1 |= STOP;
			I2C1->SR1 &= ~AF;
			return I2C_ERR_AF;
		}
		if (sr1 & ARLO)
		{
			I2C1->SR1 &= ~ARLO;
			return I2C_ERR_ARLO;
		}
		if (sr1 & ADDR)
		{
			//disable ACK
			I2C1->CR1 &= ~ACK;
			// clear ADDR
			(void)I2C1->SR1;
			(void)I2C1->SR2;
			//Generate STOP condition
			I2C1_stop();
			break;
		}
	}

	while(1) //wait for DR to get FULL
	{
		uint32_t sr1 = I2C1->SR1;
		if (sr1 & BERR)
		{
			I2C1->CR1 |= STOP;
			I2C1->SR1 &= ~BERR;
			return I2C_ERR_BERR;
		}
		if (sr1 & AF)
		{
			I2C1->CR1 |= STOP;
			I2C1->SR1 &= ~AF;
			return I2C_ERR_AF;
		}
		if (sr1 & ARLO)
		{
			I2C1->SR1 &= ~ARLO;
			return I2C_ERR_ARLO;
		}
		if (sr1 & RXNE)
		{
			break;
		}
	}

	*data = I2C1->DR;
	I2C1->CR1 |= ACK;
	I2C1->CR1 &= ~POS;

	return I2C_OK;
}

uint8_t I2C1_read2_byte(uint8_t address, uint8_t *data1, uint8_t *data2)
{
	uint8_t status = I2C1_start();
	if (status != I2C_OK)
		return status;

	I2C1->DR = (address << 1)|1;

	while(1)// wait for ADDR, it will set when ACK will be received
	{
		uint32_t sr1 = I2C1->SR1;
		if (sr1 & BERR)
		{
			I2C1->CR1 |= STOP;
			I2C1->SR1 &= ~BERR;
			return I2C_ERR_BERR;
		}
		if (sr1 & AF)
		{
			I2C1->CR1 |= STOP;
			I2C1->SR1 &= ~AF;
			return I2C_ERR_AF;
		}
		if (sr1 & ARLO)
		{
			I2C1->SR1 &= ~ARLO;
			return I2C_ERR_ARLO;
		}
		if (sr1 & ADDR)
		{
			I2C1->CR1 &= ~ACK;
			I2C1->CR1 |= POS;
			// clear ADDR
			(void)I2C1->SR1;
			(void)I2C1->SR2;
			break;
		}
	}

	while(1) //wait for complete byte transfer
	{
		uint32_t sr1 = I2C1->SR1;
		if (sr1 & BERR)
		{
			I2C1->CR1 |= STOP;
			I2C1->SR1 &= ~BERR;
			return I2C_ERR_BERR;
		}
		if (sr1 & AF)
		{
			I2C1->CR1 |= STOP;
			I2C1->SR1 &= ~AF;
			return I2C_ERR_AF;
		}
		if (sr1 & ARLO)
		{
			I2C1->SR1 &= ~ARLO;
			return I2C_ERR_ARLO;
		}
		if (sr1 & BTF)
		{
			break;
		}
	}
	I2C1_stop();


	*data1 = I2C1->DR;

	*data2 = I2C1->DR;

	I2C1->CR1 |= ACK;
	I2C1->CR1 &= ~POS;

	return I2C_OK;
}

uint8_t I2C1_read(uint8_t address, uint8_t *data, uint32_t length)
{
	if (length == 0)
	    return I2C_ERR_INVALID;

	if (length == 1)
		return I2C1_read_byte(address,data);

	if (length == 2)
		return I2C1_read2_byte(address, &data[0], &data[1]);

	I2C1->CR1 &= ~POS;
	I2C1->CR1 |= ACK;

	uint8_t status;
	status = I2C1_start();
	if (status != I2C_OK)
		return status;

	I2C1->DR = (address << 1)|1;

	while(1)// wait for ADDR, it will set when ACK will be received
	{
		uint32_t sr1 = I2C1->SR1;
		if (sr1 & BERR)
		{
			I2C1->CR1 |= STOP;
			I2C1->SR1 &= ~BERR;
			return I2C_ERR_BERR;
		}
		if (sr1 & AF)
		{
			I2C1->CR1 |= STOP;
			I2C1->SR1 &= ~AF;
			return I2C_ERR_AF;
		}
		if (sr1 & ARLO)
		{
			I2C1->SR1 &= ~ARLO;
			return I2C_ERR_ARLO;
		}
		if (sr1 & ADDR)
		{
			// clear ADDR
			(void)I2C1->SR1;
			(void)I2C1->SR2;
			break;
		}
	}

	for (uint32_t i = 0; i < length-3; i++)
	{
		while(1) //wait for DR to get FULL
		{
			uint32_t sr1 = I2C1->SR1;
			if (sr1 & BERR)
			{
				I2C1->CR1 |= STOP;
				I2C1->SR1 &= ~BERR;
				return I2C_ERR_BERR;
			}
			if (sr1 & AF)
			{
				I2C1->CR1 |= STOP;
				I2C1->SR1 &= ~AF;
				return I2C_ERR_AF;
			}
			if (sr1 & ARLO)
			{
				I2C1->SR1 &= ~ARLO;
				return I2C_ERR_ARLO;
			}
			if (sr1 & RXNE)
			{
				break;
			}
		}
		data[i] = I2C1->DR;
	}

	while(1) //wait for complete byte transfer
	{
		uint32_t sr1 = I2C1->SR1;
		if (sr1 & BERR)
		{
			I2C1->CR1 |= STOP;
			I2C1->SR1 &= ~BERR;
			return I2C_ERR_BERR;
		}
		if (sr1 & AF)
		{
			I2C1->CR1 |= STOP;
			I2C1->SR1 &= ~AF;
			return I2C_ERR_AF;
		}
		if (sr1 & ARLO)
		{
			I2C1->SR1 &= ~ARLO;
			return I2C_ERR_ARLO;
		}
		if (sr1 & BTF)
		{
			I2C1->CR1 &= ~ACK;
			break;
		}
	}
	data[length-3] = I2C1->DR;

	while(1) //wait for complete byte transfer
	{
		uint32_t sr1 = I2C1->SR1;
		if (sr1 & BERR)
		{
			I2C1->CR1 |= STOP;
			I2C1->SR1 &= ~BERR;
			return I2C_ERR_BERR;
		}
		if (sr1 & AF)
		{
			I2C1->CR1 |= STOP;
			I2C1->SR1 &= ~AF;
			return I2C_ERR_AF;
		}
		if (sr1 & ARLO)
		{
			I2C1->SR1 &= ~ARLO;
			return I2C_ERR_ARLO;
		}
		if (sr1 & BTF)
		{
			I2C1->CR1 |= STOP;
			break;
		}
	}
	data[length-2] = I2C1->DR;
	data[length-1] = I2C1->DR;

	return I2C_OK;
}

uint8_t I2C1_target_transmitter(uint8_t *data, uint32_t length)
{
	if (length == 0)
		return I2C_ERR_INVALID;

    while (!(I2C1->SR1 & ADDR)); //wait for stm to compare the address

    // ADDR cleared
    (void)I2C1->SR1;
    (void)I2C1->SR2;

    I2C1->DR = data[0];

    for (uint32_t i = 1; i < length; i++)
    {
    	while (1)
    	{
    		uint32_t sr1 = I2C1->SR1;

    		if (sr1 & AF)
    		{
    			I2C1->SR1 &= ~AF;
    			return I2C_ERR_AF;
    		}
    		if (sr1 & TXE)
    		{
    			break;
    		}
    	}

    	I2C1->DR = data[i];

    	while(1)
    	{
    		uint32_t sr1 = I2C1->SR1;

    		if (sr1 & AF)
    		{
    			I2C1->SR1 &= ~AF;
    			return I2C_ERR_AF;
    		}
    		if (sr1 & STOPF)
    		{
    			(void)I2C1->SR1;
    			I2C1->CR1 = I2C1->CR1;
    			return I2C_OK;
    		}
    	}
    }
    return I2C_OK;
}

uint8_t I2C1_target_receiver (uint8_t *data, uint32_t length)
{
	if (length == 0)
		return I2C_ERR_INVALID;

    while (!(I2C1->SR1 & ADDR))
    {
    }
    // ADDR cleared
    (void)I2C1->SR1;
    (void)I2C1->SR2;

    for (uint32_t i = 0; i < length; i++)
    {
    	while (1)
    	{
    		uint32_t sr1 = I2C1->SR1;

    		if (sr1 & RXNE)
    		{
    			break;
    		}

    	}
    	data[i]= I2C1->DR;

    }


    while(!(I2C1->SR1 & STOPF));

    (void)I2C1->SR1;
    I2C1->CR1 = I2C1->CR1;

    return I2C_OK;
}


void led_on(void)
{
	GPIOC->BSRR = (1U << 29); // LED ON when RESET
}

void led_off(void)
{
	GPIOC->BSRR = (1U << 13); //LED OFF when SET
}
