#include "button.h"

// Khởi tạo nút PA0
Button_Context btn_PA0 = {1, 0, 0, 0, BTN_EVENT_NONE, 0};
Button_Context btn_PA1 = {1, 0, 0, 0, BTN_EVENT_NONE, 0}; // Đẻ thêm sổ cho PA1
void button_state_hardware_scan(void) {
    btn_PA0.pin_state = (GPIOA->IDR & (1UL << 0)) ? 1 : 0;
	  btn_PA1.pin_state = (GPIOA->IDR & (1UL << 1)) ? 1 : 0; // Đọc điện áp PA1
}

void button_fsm_process(Button_Context *btn, unsigned int current_tick) {
    
    // KỸ THUẬT TIMESTAMP: Chỉ cho phép FSM chạy khi đã trôi qua 10ms
    if (current_tick - btn->last_update_tick >= 10) {
        
        btn->last_update_tick = current_tick; // Cập nhật mốc mới
        
        // ---- TỪ ĐÂY TRỞ XUỐNG LOGIC GIỮ NGUYÊN HOÀN TOÀN ----
        if (btn->pin_state == 0) { 
            btn->release_duration = 0;
            if (btn->press_duration < 60000) btn->press_duration++;

            if (btn->press_duration == 1000) { // 1000 nhịp * 10ms = 10s
                btn->event_code = BTN_EVENT_HOLD;
                btn->click_count = 0;
            }
        } 
        else { 
            if (btn->press_duration > 0) { 
                if (btn->press_duration > 3 && btn->press_duration < 1000) {
                    btn->click_count++;
                }
                btn->press_duration = 0;
            }

            if (btn->click_count > 0) {
                if (btn->release_duration < 60000) btn->release_duration++;

                if (btn->click_count == 2) {
                    btn->event_code = BTN_EVENT_DOUBLE_CLICK;
                    btn->click_count = 0;
                    btn->release_duration = 0;
                } 
                else if (btn->release_duration > 30) { // 30 nhịp * 10ms = 300ms
                    if (btn->click_count == 1) {
                        btn->event_code = BTN_EVENT_SINGLE_CLICK;
                    }
                    btn->click_count = 0;
                    btn->release_duration = 0;
                }
            } else {
                btn->release_duration = 0;
            }
        }
    }
}