#include "button.h"

// Đẻ ra 2 cuốn sổ khám bệnh (Mặc định: Nhả phím, thời gian = 0)
Button_Context btn_PA0 = {1, 0, 0, 0, BTN_EVENT_NONE};
Button_Context btn_PA1 = {1, 0, 0, 0, BTN_EVENT_NONE};

// =========================================================
// GIAI ĐOẠN 1: THU THẬP PHẦN CỨNG (Hardware Scan)
// =========================================================
void button_state_hardware_scan(void) {
    // Đọc chân PA0 và PA1, ghi thẳng vào hồ sơ
    btn_PA0.pin_state = (GPIOA->IDR & (1UL << 0)) ? 1 : 0;
    btn_PA1.pin_state = (GPIOA->IDR & (1UL << 1)) ? 1 : 0;
}

// =========================================================
// GIAI ĐOẠN 2: MÁY TRẠNG THÁI (Dùng chung cho mọi nút)
// =========================================================
void button_fsm_process(Button_Context *btn) {
    
    // NẾU NÚT ĐANG BỊ ĐÈ 
    if (btn->pin_state == 0) {
        btn->release_duration = 0; // Đè rồi thì xóa thời gian chờ nhả
        
        if (btn->press_duration < 60000) btn->press_duration++;

        // Bị đè đúng 10 giây (1000 nhịp * 10ms) -> CHỐT BỆNH: HOLD
        if (btn->press_duration == 1000) {
            btn->event_code = BTN_EVENT_HOLD;
            btn->click_count = 0; 
        }
    }
    // NẾU NÚT ĐANG ĐƯỢC NHẢ RA
    else {
        // Khoảnh khắc VỪA nhả tay (press_duration đang lớn hơn 0)
        if (btn->press_duration > 0) {
            // Lọc nhiễu (>30ms) và chưa bị tính là Hold (<10s)
            if (btn->press_duration > 3 && btn->press_duration < 1000) {
                btn->click_count++; // Ghi nhận 1 cú click
            }
            btn->press_duration = 0; // Nhả rồi thì xóa thời gian đè
        }

        // Khi đang chờ đợi nhịp click tiếp theo
        if (btn->click_count > 0) {
            if (btn->release_duration < 60000) btn->release_duration++;

            // TH1: Đã đủ 2 click -> CHỐT BỆNH: DOUBLE CLICK
            if (btn->click_count == 2) {
                btn->event_code = BTN_EVENT_DOUBLE_CLICK;
                btn->click_count = 0;
                btn->release_duration = 0;
            }
            // TH2: Ngâm quá 300ms mà chỉ có 1 click -> CHỐT BỆNH: SINGLE CLICK
            else if (btn->release_duration > 30) {
                if (btn->click_count == 1) {
                    btn->event_code = BTN_EVENT_SINGLE_CLICK;
                }
                btn->click_count = 0;
                btn->release_duration = 0;
            }
        } 
        else {
            btn->release_duration = 0; // Nghỉ ngơi hoàn toàn
        }
    }
}