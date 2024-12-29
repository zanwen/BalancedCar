#include "Dri_UART2.h"

void Dri_UART2_Init(void) {
    // enable clock
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

    // GPIO CONFIG
    // PA2 TX  alternate pp output
    GPIOA->CRL |= GPIO_CRL_MODE2;   // output
    GPIOA->CRL |= GPIO_CRL_CNF2_1;   // alternate pp
    GPIOA->CRL &= ~GPIO_CRL_CNF2_0;
    // PA3 RX alternate float input
    GPIOA->CRL &= ~GPIO_CRL_MODE3;   // input mode
    GPIOA->CRL &= ~GPIO_CRL_CNF3_1;
    GPIOA->CRL |= GPIO_CRL_CNF3_0;   // float input

    // USART CONFIG
    // baud rate 9600
    USART2->BRR |= (0xEA << 4 | 0x06);
    // data frame
    USART2->CR1 &= ~USART_CR1_M;   // 8 bit word length
    USART2->CR1 &= ~USART_CR1_PCE;   // diable parity
    USART2->CR2 &= ~USART_CR2_STOP;   // 1 stop bit
    // enable
    USART2->CR1 |= (USART_CR1_UE | USART_CR1_RE | USART_CR1_TE);

    // interrupt config
    USART2->CR1 |= USART_CR1_RXNEIE;
    USART2->CR1 |= USART_CR1_IDLEIE;
    
    NVIC_SetPriority(USART2_IRQn, 5);
    NVIC_EnableIRQ(USART2_IRQn);
}

static uint8_t buffer[100] = {0};
static uint8_t size = 0;
void USART2_IRQHandler(void) {
    if(USART2->SR & USART_SR_RXNE) {
        buffer[size++] = USART2->DR;
    }
    if(USART2->SR & USART_SR_IDLE) {
        USART2->DR; // dummy read for clearing IDLE flag
        buffer[size] = '\0'; // null terminate the string for printf
        Dri_UART2_ReceiveCallback(buffer, size);
        size = 0;
    }
}

__weak void Dri_UART2_ReceiveCallback(uint8_t *buf, uint8_t size) {
    
}

void Dri_UART2_SendByte(uint8_t byte) {
    // waits until transmit buffer is empty
    while ((USART2->SR & USART_SR_TXE) == 0) {}
    USART2->DR = byte;
}

void Dri_UART2_SendBytes(uint8_t* bytes, uint16_t size) {
    for (uint16_t i = 0; i < size; i++) {
        Dri_UART2_SendByte(bytes[i]);
    }
}
