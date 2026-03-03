#include "stm32f10x.h"
#include "button.h"

// Biến điều khiển tổng
volatile unsigned char xx = 0x00; 

// Hàm delay thô sơ ~10ms
void delay_ms(unsigned int ms) {
    unsigned int i, j;
    for (i = 0; i < ms; i++)
        for (j = 0; j < 2000; j++);  
}

int main(void) {
    // Cấp xung nhịp Port A, Port B, Port C
    RCC->APB2ENR |= (RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN);

    // PA0: Input Pull-up
    GPIOA->CRL &= ~(0xF << 0); 
    GPIOA->CRL |= (0x8 << 0); 
    GPIOA->ODR |= (1UL << 0);    

    // PC13: Output Push-Pull
    GPIOC->CRH &= ~(0xF << 20); 
    GPIOC->CRH |= (0x3 << 20); 
    GPIOC->ODR |= (1UL << 13); // Tắt PC13 ban đầu

    // PB12: Output Push-Pull
    GPIOB->CRH &= ~(0xF << 16); 
    GPIOB->CRH |= (0x3 << 16); 
    GPIOB->ODR |= (1UL << 12); // Tắt PB12 ban đầu

    while (1) {
        // 1. Quét trạng thái nút và chốt giá trị biến xx
        button_process();
        
        // 2. Thực thi lệnh tương ứng với giá trị xx
        button_press();         
        button_hold_toggle();   
        
        // 3. Trễ nhịp hệ thống (10ms)
        delay_ms(10); 
    }
}