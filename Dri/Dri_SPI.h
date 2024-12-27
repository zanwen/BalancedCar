#ifndef __Dri_SPI_H__
#define __Dri_SPI_H__

#include "stm32f10x.h"
// CS PA4
#define SPI_CS_HIGH() GPIOA->ODR |= GPIO_ODR_ODR4
#define SPI_CS_LOW()  GPIOA->ODR &= ~GPIO_ODR_ODR4

// SCK PA5
#define SPI_SCK_HIGH() GPIOA->ODR |= GPIO_ODR_ODR5
#define SPI_SCK_LOW()  GPIOA->ODR &= ~GPIO_ODR_ODR5

// MOSI PA7
#define SPI_MOSI_LOW()  GPIOA->ODR &= ~GPIO_ODR_ODR7
#define SPI_MOSI_HIGH() GPIOA->ODR |= GPIO_ODR_ODR7

void Dri_SPI_Init(void);
void Dri_SPI_WriteByte(uint8_t dataByte);

#endif /* __Dri_SPI_H__ */
