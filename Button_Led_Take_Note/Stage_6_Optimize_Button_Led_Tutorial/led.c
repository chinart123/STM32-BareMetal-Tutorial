#include "led.h"

// Khởi tạo 2 bóng đèn (Tắt, sáng 0)
Led_Context led_PB12 = {LED_MODE_OFF, 0, 0, 0, 0, 0};
Led_Context led_PC13 = {LED_MODE_OFF, 0, 0, 0, 0, 0};

// =========================================================
void led_set_mode(Led_Context *led, LedMode_TypeDef new_mode, unsigned int t_on, unsigned int t_off, unsigned int current_tick) {
    if (led->mode == new_mode && led->time_on == t_on && led->time_off == t_off) return; 
    
    led->mode = new_mode;
    led->time_on = t_on;
    led->time_off = t_off;
    led->last_slow_tick = current_tick;
    
    if (new_mode == LED_MODE_OFF) {
        led->brightness = 0;
    } 
    else if (new_mode == LED_MODE_ON) {
        led->brightness = LED_CFG_MAX_BRIGHTNESS;
    }
    else if (new_mode == LED_MODE_ASYNC_BLINK) {
        led->brightness = LED_CFG_MAX_BRIGHTNESS; 
        led->is_on_phase = 1; 
    }
}

// =========================================================
void led_process(Led_Context *led, unsigned int current_tick) {
    if (led->mode == LED_MODE_ASYNC_BLINK) {
        // Tự động chọn mốc thời gian dựa vào pha hiện tại
        unsigned int target_delay = (led->is_on_phase) ? led->time_on : led->time_off;
        
        if (current_tick - led->last_slow_tick >= target_delay) {
            led->last_slow_tick = current_tick;
            led->is_on_phase = !led->is_on_phase;
            led->brightness = (led->is_on_phase) ? LED_CFG_MAX_BRIGHTNESS : 0;
        }
    }
    else if (led->mode == LED_MODE_FADE_IN) {
        if (current_tick - led->last_slow_tick >= LED_CFG_FADE_SPEED) {
            led->last_slow_tick = current_tick;
            if (led->brightness < LED_CFG_MAX_BRIGHTNESS) led->brightness++;
        }
    }
    else if (led->mode == LED_MODE_FADE_OUT) {
        if (current_tick - led->last_slow_tick >= LED_CFG_FADE_SPEED) {
            led->last_slow_tick = current_tick;
            if (led->brightness > 0) led->brightness--;
        }
    }
}

// =========================================================
void led_hardware_update(unsigned int current_tick) {
    unsigned int pwm_phase = current_tick % LED_CFG_MAX_BRIGHTNESS; 
    
    // Cập nhật PB12 (Active-Low: Kéo xuống 0 là sáng)
    if (pwm_phase < led_PB12.brightness) GPIOB->BRR = (1UL << 12);
    else                                 GPIOB->BSRR = (1UL << 12);

    // Cập nhật PC13 (Active-Low: Kéo xuống 0 là sáng)
    if (pwm_phase < led_PC13.brightness) GPIOC->BRR = (1UL << 13);
    else                                 GPIOC->BSRR = (1UL << 13);
}