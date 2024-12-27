#include "Dri_I2C.h"
#include "logger.h"

#define TIMEOUT_COUNT 0xFFFF
#define OK            0
#define FAIL          1

void Dri_I2C_Init(void) {
    // 1. GPIO时钟配置
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;

    // 2. 配置GPIO工作模式为复用开漏输出 mode=11 cnf=11
    GPIOB->CRH |= (GPIO_CRH_MODE11 | GPIO_CRH_CNF11);
    GPIOB->CRH |= (GPIO_CRH_MODE10 | GPIO_CRH_CNF10);
    // 特别注意，如下操作无法得到预期结果，GPIO工作不正常
    // GPIOB->CRH |= (GPIO_CRH_CNF10 | GPIO_CRH_CNF11);
    // GPIOB->CRH |= (GPIO_CRH_MODE10 | GPIO_CRH_MODE11);
    // 如下则是可以的
    // GPIOB->CRH |= (GPIO_CRH_MODE11 | GPIO_CRH_CNF11 | GPIO_CRH_MODE10 |
    // GPIO_CRH_CNF10);

    // 3. 配置外设为I2C模式（默认就是I2C模式，1-SMBUS模式）
    I2C2->CR1 &= ~I2C_CR1_SMBUS;

    // 4. 配置外设时钟
    I2C2->CR2 |= 36; // 必须小于等于APB1的最大时钟频率36MHz

    // 5. 配置外设模式：I2C标准模式（100kHz）、SCL时钟频率（100kHz）
    I2C2->CCR &= ~I2C_CCR_FS; // 标准模式
    I2C2->CCR |= 180;         // SCL高低电平周期5us / 外设时钟周期(1/36us) = 5 * 36

    // 6. 配置TRISE-SCL最大上升时间
    I2C2->TRISE &= ~I2C_TRISE_TRISE; // 清零
    I2C2->TRISE |= 37; // I2C标准模式最大上升时间1us / 外设时钟周期(1/36us) + 1 = 36 + 1

    // 7. 使能外设
    I2C2->CR1 |= I2C_CR1_PE; // Peripheral(I2C) Enable
}

void Dri_I2C_Start(void) {
    // 产生起始信号（当BUSY=1时不会立即产生）
    I2C2->CR1 |= I2C_CR1_START;
    uint16_t timeout = TIMEOUT_COUNT;
    // 起始信号产生后，由硬件置位SB
    while ((I2C2->SR1 & I2C_SR1_SB) == 0 && timeout > 0) {
        timeout--;
    }
    if (timeout == 0) {
        LOG_ERROR("Dri_I2C_Start timeout, timeout = %d", timeout)
    }
    // 紧接着发送设备地址会写DR，读SR1写DR序列会清除SB，因此这里不需要手动清除SB
}

void Dri_I2C_SendAddr(uint8_t addr) {
    uint16_t timeout = TIMEOUT_COUNT;
    // 产生起始信号后，DR肯定为空，可以将从设备地址直接写入DR
    I2C2->DR = addr;
    // 等待硬件置位ADDR（表明从设备地址以发出并收到了相应从设备的ACK）
    while ((I2C2->SR1 & I2C_SR1_ADDR) == 0 && timeout > 0) {
        timeout--;
    }
    if (timeout == 0) {
        LOG_ERROR("Dri_I2C_SendAddr timeout, addr = %d", addr)
    }

    // 清除ADDR标志位操作序列：读SR1后跟随一个读SR2
    I2C2->SR2;
}

void Dri_I2C_SendByte(uint8_t data) {
    uint16_t timeout = TIMEOUT_COUNT;
    // 等待发送缓冲区非空
    while ((I2C2->SR1 & I2C_SR1_TXE) == 0 && timeout > 0) {
        timeout--;
    }
    if (timeout == 0) {
        LOG_ERROR("Dri_I2C_SendByte wait TXE timeout, data = %d", data)
    }
    // 将数据传输到发送缓冲区，同时读SR1跟随一个写DR将清除TXE标志
    I2C2->DR = data;

    // 等待当前字节传输完成且收到ACK（可选，可以在超时后针对当前字节的NACK做异常处理）
    timeout = TIMEOUT_COUNT;
    while ((I2C2->SR1 & I2C_SR1_BTF) == 0 && timeout > 0) {
        timeout--;
    }
    if (timeout == 0) {
        LOG_ERROR("Dri_I2C_SendByte wait BTF timeout, data = %d", data)
    }
    // 这里不需要手动清除BTF，下一个字节的写DR会清除BTF，停止信号也会清除BTF
}

void Dri_I2C_Stop4Send(void) {
    uint16_t timeout = TIMEOUT_COUNT;
    // 等待条件: 发送缓冲区为空
    while (((I2C2->SR1 & I2C_SR1_TXE) == 0) && timeout > 0) {
        timeout--;
    }
    if (timeout == 0) {
        LOG_ERROR("Dri_I2C_Stop4Send wait TXE timeout")
    }
    I2C2->CR1 |= I2C_CR1_STOP;
}

void Dri_I2C_EnableStop(void) { I2C2->CR1 |= I2C_CR1_STOP; }

uint8_t Dri_I2C_ReceiveByte(void) {
    uint16_t timeout = TIMEOUT_COUNT;
    while ((I2C2->SR1 & I2C_SR1_RXNE) == 0 && timeout > 0) {
        timeout--;
    }
    if (timeout == 0) {
        LOG_ERROR("Dri_I2C_ReceiveByte wait RXNE timeout, timeout = %d", timeout)
    }
    return I2C2->DR;
}

void Dri_I2C_EnableAutoAck(void) { I2C2->CR1 |= I2C_CR1_ACK; }

void Dri_I2C_DisableAutoAck(void) { I2C2->CR1 &= ~I2C_CR1_ACK; }

void Dri_I2C_Test(void) {
    Dri_I2C_Start();
    Dri_I2C_SendAddr(0xA0);
    Dri_I2C_SendByte(0x0);
    Dri_I2C_SendByte('a');
    Dri_I2C_SendByte('b');
    Dri_I2C_Stop4Send();

    //delay_ms(5);

    Dri_I2C_Start();
    Dri_I2C_SendAddr(0xA0);
    Dri_I2C_SendByte(0x00);

    Dri_I2C_EnableAutoAck();
    Dri_I2C_Start();
    Dri_I2C_SendAddr(0xA0 + 1);
    Dri_I2C_ReceiveByte();
    Dri_I2C_DisableAutoAck();
    Dri_I2C_EnableStop();
    Dri_I2C_ReceiveByte();
}
