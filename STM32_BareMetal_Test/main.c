#include "stm32f10x.h"
#include "hal_timer_pwm.h"

int main(void) {
    // Không cần bật ngắt toàn cục __enable_irq()
    // Không cần khởi tạo TIM2 timer_init()
    
    // Chỉ bật duy nhất TIM3 (Phần cứng băm xung)
    HAL_TIM3_PWM_Init(); 
    
    // Cài đặt cứng vạch ranh giới CCR = 200
    HAL_TIM3_PWM_SetDuty(3, 200); // Kênh 3 (PB0) sáng 20%
    HAL_TIM3_PWM_SetDuty(4, 800); // Kênh 4 (PB1) sáng 80% cho nó khác biệt

    while(1) {
        // Vòng lặp trống rỗng. CPU đi ngủ khò khò.
        // Phần cứng tự động băm xung ở chân PB0 và PB1 mãi mãi.
    }
}