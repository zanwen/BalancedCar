#include "Dri_UART1.h"
#include "stdio.h"

void Dri_UART1_Init(void) {
    // enable clock
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

    // GPIO CONFIG
    // PA9 TX  alternate pp output
    GPIOA->CRH |= GPIO_CRH_MODE9;   // output
    GPIOA->CRH |= GPIO_CRH_CNF9_1;   // alternate pp
    GPIOA->CRH &= ~GPIO_CRH_CNF9_0;
    // PA10 RX alternate float input
    GPIOA->CRH &= ~GPIO_CRH_MODE10;   // input mode
    GPIOA->CRH &= ~GPIO_CRH_CNF10_1;
    GPIOA->CRH |= GPIO_CRH_CNF10_0;   // float input

    // AFIO CONFIG
    AFIO->MAPR &= ~AFIO_MAPR_USART1_REMAP;

    // USART CONFIG
    // baud rate
    USART1->BRR &= ~USART_BRR_DIV_Mantissa;
    USART1->BRR &= ~USART_BRR_DIV_Fraction;
    USART1->BRR |= (0x27 << 4 | 0x01);
    //USART1->BRR = 0x271;
    // data frame
    USART1->CR1 &= ~USART_CR1_M;   // 8 bit word length
    USART1->CR1 &= ~USART_CR1_PCE;   // diable parity
    USART1->CR2 &= ~USART_CR2_STOP;   // 1 stop bit
    // enable
    USART1->CR1 |= (USART_CR1_UE | USART_CR1_RE | USART_CR1_TE);

    // interrupt config
    USART1->CR1 |= USART_CR1_RXNEIE;
    USART1->CR1 |= USART_CR1_IDLEIE;
    
    NVIC_SetPriority(USART1_IRQn, 1);
    NVIC_EnableIRQ(USART1_IRQn);
}

static uint8_t buffer[100] = {0};
static uint8_t size = 0;
void USART1_IRQHandler(void) {
    if(USART1->SR & USART_SR_RXNE) {
        buffer[size++] = USART1->DR;
    }
    if(USART1->SR & USART_SR_IDLE) {
        USART1->DR; // dummy read for clearing IDLE flag
        buffer[size] = '\0'; // null terminate the string for printf
        Dri_UART1_ReceiveCallback(buffer, size);
        size = 0;
    }
}

__weak void Dri_UART1_ReceiveCallback(uint8_t *buf, uint8_t size) {
    
}

void Dri_UART1_SendByte(uint8_t byte) {
    // waits until transmit buffer is empty
    while ((USART1->SR & USART_SR_TXE) == 0) {}
    USART1->DR = byte;
}

void Dri_UART1_SendBytes(uint8_t* bytes, u16 size) {
    for (uint16_t i = 0; i < size; i++) {
        Dri_UART1_SendByte(bytes[i]);
    }
}

// printf retarget
int fputc(int ch, FILE * stream) {
    Dri_UART1_SendByte((uint8_t)ch);
    return ch;
}
