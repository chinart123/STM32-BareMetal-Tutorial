#include "button.h"

// Biến nội bộ cho Double Click
unsigned int  press_time = 0;  // Đo thời gian giữ phím
unsigned int  gap_time = 0;    // Đo khoảng thời gian giữa 2 lần bấm
unsigned char is_pressing = 0; // Cờ báo trạng thái vật lý
unsigned char click_count = 0; // Đếm số lần nhả phím hợp lệ

// =========================================================
// 1. HÀM QUÉT NÚT (Nhận diện Double Click)
// =========================================================
void button_process_double(void) {
    // Nếu nút đang BỊ NHẤN (Mức 0)
    if ((GPIOA->IDR & (1UL << 0)) == 0) { 
        is_pressing = 1;
        gap_time = 0; // Đang bấm thì reset thời gian chờ
        
        if (press_time < 60000) {
            press_time++; // Tăng thời gian giữ phím
        }
    } 
    // Nếu nút ĐÃ NHẢ RA (Mức 1)
    else { 
        if (is_pressing == 1) { // Bắt đúng khoảnh khắc vừa nhả tay
            is_pressing = 0;
            
            // Điều kiện click hợp lệ: Bấm đủ sâu (>30ms) nhưng không quá lâu (<500ms)
            if (press_time > 3 && press_time < 50) {
                click_count++; // Ghi nhận 1 cú click
                
                // NẾU ĐÃ ĐẠT ĐỦ 2 CÚ CLICK LIÊN TIẾP
                if (click_count == 2) {
                    xx = 0x03;       // NHẢ GIÁ TRỊ 0x03 NHƯ ĐỀ BÀI YÊU CẦU!
                    click_count = 0; // Reset để đón chu kỳ mới
                }
            } else {
                click_count = 0; // Bấm sai nhịp (giữ quá lâu) -> Xóa chuỗi click
            }
            press_time = 0;
        } 
        else { // Đang trong trạng thái nhả tay chờ đợi
            if (click_count > 0) {
                gap_time++;
                // Nếu người dùng ngâm quá 300ms (30 nhịp) mà không bấm phát thứ 2
                if (gap_time > 30) {
                    click_count = 0; // Hủy bỏ chuỗi click (coi như đó là bấm đơn)
                    gap_time = 0;
                }
            }
        }
    }
}

// =========================================================
// 2. HÀM KIỂM TRA GIÁ TRỊ (Dùng chung)
// =========================================================
unsigned char button_check(unsigned char target_xx) {
    if (xx == target_xx) {
        return 1;
    }
    return 0;     
}

// =========================================================
// 3. HÀM THỰC THI DOUBLE CLICK
// =========================================================
void button_double_click(void) {
    // Nếu phát hiện xx = 0x03
    if (button_check(0x03) == 1) {
        
        GPIOC->ODR ^= (1UL << 13); // Đảo trạng thái LED PC13 để báo hiệu
        
        // [QUAN TRỌNG] Nuốt sự kiện: Phải reset xx về 0
        // Nếu không reset, vòng lặp 10ms tiếp theo sẽ thấy xx vẫn bằng 0x03 và chớp LED loạn xạ
        xx = 0x00; 
    }
}