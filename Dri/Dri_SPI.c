#include "Dri_SPI.h"

void Dri_SPI_Init(void) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    // CS-PA4 和 SCK-PA5，MOSI-PA7 通用推挽 MODE=11 CNF=00
    GPIOA->CRL |= GPIO_CRL_MODE4;
    GPIOA->CRL &= ~GPIO_CRL_CNF4;

    GPIOA->CRL |= GPIO_CRL_MODE5;
    GPIOA->CRL &= ~GPIO_CRL_CNF5;

    GPIOA->CRL |= GPIO_CRL_MODE7;
    GPIOA->CRL &= ~GPIO_CRL_CNF7;

    // SPI模式0（极性=0，相位=0） 空闲状态 CS拉高（低电平使能），SCK拉低
    SPI_CS_HIGH();
    SPI_SCK_LOW();
}

void Dri_SPI_Start(void) {}
void Dri_SPI_Stop(void) {}

void Dri_SPI_WriteByte(uint8_t dataByte) {
    SPI_CS_LOW();
    for (uint8_t i = 0; i < 8; i++) {
        SPI_SCK_LOW();
        if (dataByte & 0x80) {
            SPI_MOSI_HIGH();
        } else {
            SPI_MOSI_LOW();
        }
        dataByte <<= 1;
        // 主设备拉高SCK（双方发送数据阶段），让从设备读取数据
        SPI_SCK_HIGH();
    }
    SPI_CS_HIGH();
}
