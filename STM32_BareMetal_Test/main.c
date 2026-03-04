#include "stm32f10x.h"
#include "hal_timer_pwm.h"

volatile uint32_t timer_ms = 0; // Biến đếm nhịp TIM2
uint32_t last_fade_tick = 0;
int16_t brightness = 0;
int8_t fade_direction = 1;      // 1 là tăng, -1 là giảm

// 1. HÀM PHỤC VỤ NGẮT TIM2 (Trái tim của hệ thống)
void TIM2_IRQHandler(void) {
    if (TIM2->SR & TIM_SR_UIF) {
        TIM2->SR &= ~TIM_SR_UIF; 
        timer_ms++;              
    }
}

// 2. HÀM KHỞI TẠO TIM2
void timer_init(void) {
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;   
    TIM2->PSC = 71;                       
    TIM2->ARR = 999;                      
    
    TIM2->EGR |= TIM_EGR_UG;              
    TIM2->SR &= ~TIM_SR_UIF;              

    TIM2->DIER |= TIM_DIER_UIE;           
    NVIC_EnableIRQ(TIM2_IRQn);            
    TIM2->CR1 |= TIM_CR1_CEN;             
}

int main(void) {
    // BẮT BUỘC: Mở khóa ngắt toàn cục cho CPU
    __enable_irq();      
     
    // GỌI HÀM KHỞI TẠO (Không có chữ void)
    timer_init();        
    HAL_TIM3_PWM_Init(); 

    while(1) {
        // Cứ mỗi 2ms (Tốc độ Fade), ta cập nhật độ sáng 1 lần
        if (timer_ms - last_fade_tick >= 2) {
            last_fade_tick = timer_ms;

            // Tính toán Logic Breathing
            brightness += fade_direction;
            
            if (brightness >= 999) {
                brightness = 999;
                fade_direction = -1; // Đụng nóc thì quay đầu giảm
            } else if (brightness <= 0) {
                brightness = 0;
                fade_direction = 1;  // Chạm đáy thì quay đầu tăng
            }

            // Đẩy lệnh xuống tầng Hardware
            HAL_TIM3_PWM_SetDuty(3, brightness); 
            HAL_TIM3_PWM_SetDuty(4, 999 - brightness); 
        }
    }
}