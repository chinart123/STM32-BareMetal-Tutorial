#ifndef LED_H
#define LED_H

#include "stm32f10x.h"

// 1. Cấu hình hằng số (Những thứ dùng chung cho mọi LED)
typedef enum {
    LED_CFG_MAX_BRIGHTNESS = 20,   // Chu kỳ PWM (20 nhịp)
    LED_CFG_FADE_SPEED     = 50    // Tốc độ Fading chung (50ms/bậc)
} LedConfig_t;

// 2. Các chế độ hoạt động
typedef enum {
    LED_MODE_OFF = 0,
    LED_MODE_ON,          // Sáng hẳn (Không chớp)
    LED_MODE_FADE_IN,     // Sáng dần
    LED_MODE_FADE_OUT,    // Tối dần
    LED_MODE_ASYNC_BLINK  // Chớp lệch pha
} LedMode_TypeDef;

// 3. Hồ sơ Đèn LED (Đã nâng cấp Tham số động)
typedef struct {
    LedMode_TypeDef mode;
    unsigned int    brightness;     
    unsigned int    last_slow_tick; 
    unsigned char   is_on_phase;    
    
    // THAM SỐ ĐỘNG (Dùng cho ASYNC_BLINK)
    unsigned int    time_on;        // Khách muốn sáng bao lâu?
    unsigned int    time_off;       // Khách muốn tắt bao lâu?
} Led_Context;

// Khai báo 2 con LED để main.c sử dụng
extern Led_Context led_PB12;
extern Led_Context led_PC13;

// Hàm giao tiếp
void led_set_mode(Led_Context *led, LedMode_TypeDef new_mode, unsigned int t_on, unsigned int t_off, unsigned int current_tick);
void led_process(Led_Context *led, unsigned int current_tick);
void led_hardware_update(unsigned int current_tick); // Tách riêng phần cứng cho sạch

#endif // LED_H