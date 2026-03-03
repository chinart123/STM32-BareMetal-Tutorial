#include "led.h"

// Khởi tạo PB12: Tắt hoàn toàn, độ sáng 0
Led_Context led_PB12 = {LED_MODE_OFF, 0, 0, 0};

// =========================================================
// HÀM 1: CẬP NHẬT CHẾ ĐỘ MỚI (CHỈ GỌI KHI CÓ SỰ THAY ĐỔI)
// =========================================================
void led_set_mode(Led_Context *led, LedMode_TypeDef new_mode, unsigned int current_tick) {
    if (led->mode == new_mode) return; // Nếu giống chế độ cũ thì bỏ qua
    
    led->mode = new_mode;
    led->last_slow_tick = current_tick; // Bắt đầu tính giờ từ lúc nhận lệnh
    
    if (new_mode == LED_MODE_OFF) {
        led->brightness = 0;
    } 
    else if (new_mode == LED_MODE_ASYNC_BLINK) {
        led->brightness = 20; // Sáng Max
        led->is_on_phase = 1; // Bắt đầu bằng pha sáng
    }
    // Đối với FADE_IN và FADE_OUT, ta CỐ TÌNH giữ nguyên brightness hiện tại.
    // Điều này giúp LED chuyển trạng thái cực mượt (Ví dụ đang tối dần nửa chừng 
    // mà bấm FADE_IN thì nó sẽ sáng dần lên từ điểm đó, không bị giật chớp).
}

// =========================================================
// HÀM 2: MÁY TRẠNG THÁI & SOFTWARE PWM (GỌI LIÊN TỤC TRONG MAIN)
// =========================================================
void led_process(Led_Context *led, unsigned int current_tick) {
    
    // --- KHỐI A: LOGIC NHỊP CHẬM (Đổi độ sáng / Đổi pha) ---
    if (led->mode == LED_MODE_ASYNC_BLINK) {
        // Sáng 1s (1000ms), Tắt 0.5s (500ms)
        unsigned int delay_target = (led->is_on_phase) ? 1000 : 500;
        
        if (current_tick - led->last_slow_tick >= delay_target) {
            led->last_slow_tick = current_tick;
            led->is_on_phase = !led->is_on_phase;          // Đảo pha
            led->brightness = (led->is_on_phase) ? 20 : 0; // Gán Max hoặc Min
        }
    }
    else if (led->mode == LED_MODE_FADE_IN) {
        // Mỗi 50ms tăng độ sáng 1 bậc. Cần 50 * 20 = 1000ms (1s) để sáng tối đa.
        if (current_tick - led->last_slow_tick >= 50) {
            led->last_slow_tick = current_tick;
            if (led->brightness < 20) led->brightness++;
        }
    }
    else if (led->mode == LED_MODE_FADE_OUT) {
        // Mỗi 50ms giảm độ sáng 1 bậc.
        if (current_tick - led->last_slow_tick >= 50) {
            led->last_slow_tick = current_tick;
            if (led->brightness > 0) led->brightness--;
        }
    }

    // --- KHỐI B: SOFTWARE PWM 50Hz (CHẠY LIÊN TỤC) ---
    // Tạo vòng lặp vô tận từ 0 -> 19
    unsigned int pwm_phase = current_tick % 20; 
    
    // So sánh để băm xung (Mạch active-low: Kéo xuống 0 là sáng)
    if (pwm_phase < led->brightness) {
        GPIOB->BRR = (1UL << 12);  // Sáng PB12
    } else {
        GPIOB->BSRR = (1UL << 12); // Tắt PB12
    }
}