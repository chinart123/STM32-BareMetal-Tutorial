#include "stm32f10x.h"
#include "hal_timer_pwm.h"
//chưa cấp phát bộ nhớ, bỏ extern đi
//extern volatile uint32_t timer_ms;  // Biến đếm nhịp TIM2 của chú
volatile uint32_t timer_ms; // Biến đếm nhịp TIM2 của chú

uint32_t last_fade_tick = 0;
int16_t brightness = 0;
int8_t fade_direction = 1; // 1 là tăng, -1 là giảm

int main(void) {
    //timer_init();        // Bật ngắt TIM2 (Cung cấp nhịp timer_ms)
		// 1. Thêm dòng nguyên mẫu hàm này vào:
		void timer_init(void);
    HAL_TIM3_PWM_Init(); // Bật phần cứng PWM TIM3

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
            HAL_TIM3_PWM_SetDuty(3, brightness); // Kênh 3 nhịp thở chậm
            
            // Thích thì cho Kênh 4 nhịp thở ngược lại
            HAL_TIM3_PWM_SetDuty(4, 999 - brightness); 
        }
        
        // Nhờ kiến trúc non-blocking, CPU chú rảnh rang 100% để làm việc khác ở đây
        // Ví dụ: read_sensor_I2C();
    }
}