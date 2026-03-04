#include "stm32f10x.h"
#include "hal_timer_pwm.h"

int16_t brightness = 0;
int8_t fade_direction = 1;      

int main(void) {
    HAL_TIM3_PWM_Init(); // Bật phần cứng PWM TIM3

    while(1) {
        // Tăng giảm độ sáng
        brightness += fade_direction;
        
        if (brightness >= 999) {
            brightness = 999;
            fade_direction = -1; 
        } else if (brightness <= 0) {
            brightness = 0;
            fade_direction = 1;  
        }

        // Đẩy xuống phần cứng
        HAL_TIM3_PWM_SetDuty(3, brightness); 
        HAL_TIM3_PWM_SetDuty(4, 999 - brightness); 
        
        // Vòng lặp delay "ngu" thay cho TIM2 để KeilC không bị kẹt
        for (volatile int i = 0; i < 5000; i++); 
    }
}