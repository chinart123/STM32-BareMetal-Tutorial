#include "button.h"

// Biến nội bộ tính toán thời gian
unsigned char toggle_state = 0;     
unsigned int  hold_time = 0;        
unsigned char is_pressing = 0;      
unsigned int  blink_timer = 0;      

// =========================================================
// 1. HÀM QUÉT NÚT (Nhận diện thao tác và gán biến xx)
// =========================================================
void button_process(void) {
    // Đọc chân PA0: Nếu bị kéo xuống 0 (Đang nhấn)
    if ((GPIOA->IDR & (1UL << 0)) == 0) { 
        is_pressing = 1;
        
        if (hold_time < 60000) {
            hold_time++; // Tăng mỗi 10ms
        }
        
        // Nhấn giữ đúng 10 giây (1000 * 10ms = 10000ms)
        if (hold_time == 1000) {
            xx = 0x04; 
        }
    } 
    // Nếu chân PA0 ở mức 1 (Đã nhả tay)
    else { 
        if (is_pressing == 1) { // Bắt khoảnh khắc vừa mới nhả
            
            // Lọc nhiễu >30ms và <10s (Nhấn nhả bình thường)
            if (hold_time > 3 && hold_time < 1000) {
                toggle_state = !toggle_state; 
                
                if (toggle_state == 1) {
                    xx = 0x01;
                } else {
                    xx = 0x02;
                }
            }
            // Reset chu kỳ
            is_pressing = 0;
            hold_time = 0;
        }
    }
}

// =========================================================
// 2. HÀM KIỂM TRA GIÁ TRỊ (Kiểm chứng tham số đầu vào)
// =========================================================
unsigned char button_check(unsigned char target_xx) {
    if (xx == target_xx) {
        return 1;
    }
    return 0;     
}

// =========================================================
// 3. HÀM XỬ LÝ LED PC13 (xx = 0x01 hoặc xx = 0x02)
// =========================================================
void button_press(void) {
    // Kiểm tra lồng hàm
    if (button_check(0x01) == 1) {
        GPIOC->BRR = (1UL << 13);  // Kéo PC13 xuống 0 -> Bật LED
    } 
    else if (button_check(0x02) == 1) {
        GPIOC->BSRR = (1UL << 13); // Kéo PC13 lên 1 -> Tắt LED
    }
}

// =========================================================
// 4. HÀM XỬ LÝ LED PB12 (xx = 0x04)
// =========================================================
void button_hold_toggle(void) {
    if (button_check(0x04) == 1) {
        blink_timer++;
        
        // Nếu đủ 50 vòng (50 * 10ms = 500ms)
        if (blink_timer >= 50) {
            GPIOB->ODR ^= (1UL << 12); // Đảo trạng thái bit thứ 12 của Port B
            blink_timer = 0;           // Reset đếm chớp
        }
    } 
    else {
        // Đảm bảo PB12 luôn tắt nếu không ở chế độ 0x04
        GPIOB->BSRR = (1UL << 12); // Kéo PB12 lên 1 -> Tắt LED
        blink_timer = 0;
    }
}