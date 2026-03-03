#include "stm32f10x.h"
#include "button.h"

volatile unsigned char xx = 0x00; 

void delay_ms(unsigned int ms) {
    unsigned int i, j;
    for (i = 0; i < ms; i++)
        for (j = 0; j < 2000; j++);  
}

int main(void) {
    // Khởi tạo (Giữ nguyên cấu hình I/O)
    RCC->APB2ENR |= (RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN);

    GPIOA->CRL &= ~(0xF << 0); GPIOA->CRL |= (0x8 << 0); GPIOA->ODR |= (1UL << 0);    
    GPIOC->CRH &= ~(0xF << 20); GPIOC->CRH |= (0x3 << 20); GPIOC->ODR |= (1UL << 13); 
    GPIOB->CRH &= ~(0xF << 16); GPIOB->CRH |= (0x3 << 16); GPIOB->ODR |= (1UL << 12); 

    while (1) {
        // Quét nút chuyên biệt cho chức năng Double Click
        button_process_double();
        
        // Thực thi kiểm tra và đảo LED
        button_double_click();   
        
        // Trễ 10ms
        delay_ms(10); 
    }
}