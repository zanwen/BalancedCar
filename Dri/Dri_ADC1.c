#include "Dri_ADC1.h"
#include "Com_Delay.h"
#include "logger.h"

void Dri_ADC1_Init(void) {
    // ADC输入时钟配置
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
    // APB2 6分频 => 12M，ADC最大频率不能超过14M
    RCC->CFGR |= RCC_CFGR_ADCPRE_1;
    RCC->CFGR &= ~RCC_CFGR_ADCPRE_0;

    // GPIO工作模式配置为模拟输入 MODE=00 CNF=00
    GPIOA->CRL &= ~(GPIO_CRL_MODE0 | GPIO_CRL_CNF0);

    // 编排常规组通道序列
    // 通道数量为1 => 0000
    ADC1->SQR1 &= ~ADC_SQR1_L;
    // 将通道1配置为转换序列第一个 => 00001
    ADC1->SQR3 &= ~ADC_SQR3_SQ1;
    ADC1->SQR3 |= ADC_SQR3_SQ1_0;
    // 配置通道的采样时间 239.5 cycles => 111
    ADC1->SMPR2 &= ~ADC_SMPR2_SMP1;
    ADC1->SMPR2 &= ~ADC_SMPR2_SMP1_2;
    ADC1->SMPR2 |= ADC_SMPR2_SMP1_1;
    ADC1->SMPR2 &= ~ADC_SMPR2_SMP1_0;

    // ADC控制相关
    // 关闭扫描模式，常规组只有一个通道
    ADC1->CR1 &= ~ADC_CR1_SCAN;
    // 关闭连续模式，循环采样可调电阻电压
    ADC1->CR2 &= ~ADC_CR2_CONT;
    // 数据对齐：右对齐
    ADC1->CR2 &= ~ADC_CR2_ALIGN;
    // 使能外部触发ADC采样
    ADC1->CR2 |= ADC_CR2_EXTTRIG;
    // 外部触发源选择软件触发 => 111
    ADC1->CR2 |= ADC_CR2_EXTSEL;

    // 唤醒ADC
    ADC1->CR2 |= ADC_CR2_ADON;
    // 等待至少两个ADC时钟周期后开始执行校准
    for_delay_ms(1);
    ADC1->CR2 |= ADC_CR2_CAL;
    // 等待校准结束
    while (ADC1->CR2 & ADC_CR2_CAL) {
    }
    // 软件触发开始转换
    ADC1->CR2 |= ADC_CR2_SWSTART;
    // 等待第一次转换完成
    while ((ADC1->SR & ADC_SR_EOC) == 0) {
    }
}

double Dri_ADC1_ReadVoltage(void) {
    // 软件触发开始转换
    ADC1->CR2 |= ADC_CR2_SWSTART;
    // 等待第一次转换完成
    while ((ADC1->SR & ADC_SR_EOC) == 0) {
    }
    uint16_t adc_val = ADC1->DR;
    double voltage   = (adc_val * 3.3 / 4095) * 4;
    // LOG_DEBUG("adc_val = %d, voltage = %.2fV", adc_val, voltage);
    return voltage;
}
