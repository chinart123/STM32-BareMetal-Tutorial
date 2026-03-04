#include "stm32f10x.h"
#include "hal_timer_pwm.h"

// Biến cho LED 1 (Thở nhịp bình thường)
int16_t brightness_1 = 0;
int8_t fade_dir_1 = 1;

// Biến cho LED 2 (Xuất phát từ đỉnh, thở nhanh gấp đôi)
int16_t brightness_2 = 999;
int8_t fade_dir_2 = -1;

int main(void) {
    // Chỉ bật phần cứng PWM TIM3, cất TIM2 đi để Simulator không bị kẹt
    HAL_TIM3_PWM_Init(); 

    while(1) {
        // --- Xử lý Logic LED 1 (Kênh 3) ---
        brightness_1 += fade_dir_1; // Bước nhảy là 1
        if (brightness_1 >= 999) {
            brightness_1 = 999;
            fade_dir_1 = -1; 
        } else if (brightness_1 <= 0) {
            brightness_1 = 0;
            fade_dir_1 = 1;  
        }

        // --- Xử lý Logic LED 2 (Kênh 4) ---
        brightness_2 += (fade_dir_2 * 2); // Bước nhảy là 2 -> Chạy nhanh gấp đôi
        if (brightness_2 >= 999) {
            brightness_2 = 999;
            fade_dir_2 = -1; 
        } else if (brightness_2 <= 0) {
            brightness_2 = 0;
            fade_dir_2 = 1;  
        }

        // --- Đẩy dữ liệu xuống thanh ghi ---
        HAL_TIM3_PWM_SetDuty(3, brightness_1); 
        HAL_TIM3_PWM_SetDuty(4, brightness_2); 
        
        // --- Delay "ngu" thay cho TIM2 để Simulator chạy được ---
        for (volatile int i = 0; i < 2000; i++); 
    }
}