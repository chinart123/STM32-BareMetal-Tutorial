#include "button.h"

unsigned int  press_time = 0;  
unsigned int  gap_time = 0;    
unsigned char is_pressing = 0; 
unsigned char click_count = 0; 

// =========================================================
// 1. HÀM QUÉT NÚT (Nhận diện Double Click)
// =========================================================
void button_process_double(void) {
    if ((GPIOA->IDR & (1UL << 0)) == 0) { 
        // Vừa mới chạm tay vào nút là lập tức xóa xx cũ đi để dọn đường
        if (is_pressing == 0) {
            if (xx == 0x03) xx = 0x00; 
        }
        
        is_pressing = 1;
        gap_time = 0; 
        
        if (press_time < 60000) press_time++; 
    } 
    else { 
        if (is_pressing == 1) { 
            is_pressing = 0;
            
            if (press_time > 3 && press_time < 50) {
                click_count++; 
                
                if (click_count == 2) {
                    xx = 0x03; // BẬT LÊN 0x03 VÀ GIỮ NGUYÊN HIỆN TRƯỜNG Ở ĐÓ!
                    click_count = 0; 
                }
            } else {
                click_count = 0; 
            }
            press_time = 0;
        } 
        else { 
            if (click_count > 0) {
                gap_time++;
                if (gap_time > 30) { // Quá 300ms không bấm nhát thứ 2 -> Hủy
                    click_count = 0; 
                    gap_time = 0;
                }
            }
        }
    }
}

unsigned char button_check(unsigned char target_xx) {
    if (xx == target_xx) return 1;
    return 0;     
}

// =========================================================
// 3. HÀM THỰC THI DOUBLE CLICK
// =========================================================
void button_double_click(void) {
    // Biến tĩnh (static) để ghi nhớ việc đảo LED đã làm chưa
    static unsigned char executed = 0; 

    if (button_check(0x03) == 1) {
        // Nếu thấy 0x03 VÀ chưa thực thi
        if (executed == 0) {
            GPIOC->ODR ^= (1UL << 13); // Đảo LED 1 phát duy nhất
            executed = 1;              // Cắm cờ "Đã làm", không đảo thêm nữa
            
            // TUYỆT ĐỐI KHÔNG XÓA xx = 0x00 Ở ĐÂY NỮA!
        }
    } 
    else {
        // Khi xx bị reset về 0x00 (do người dùng bấm nút mới), ta hạ cờ xuống
        executed = 0; 
    }
}