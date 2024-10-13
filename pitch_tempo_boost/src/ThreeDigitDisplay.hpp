#ifndef THREE_DIGIT_DISPLAY_HPP
#define THREE_DIGIT_DISPLAY_HPP

#include "render_matrix/Sprite.hpp"
#include "Resources.hpp"

struct ThreeDigitDisplay {

    ThreeDigitDisplay() {
        spr_digit_hundreds_place_ = (Sprite){};
        spr_digit_tens_place_ = (Sprite){};
        spr_digit_ones_place_ = (Sprite){};
    }
    ~ThreeDigitDisplay() = default;

    void initialize() {

        spr_digit_hundreds_place_.set_bitmap_image(&image_char_1_w3);
        spr_digit_hundreds_place_.x = 0;
        spr_digit_hundreds_place_.y = 1;

        spr_digit_tens_place_.set_bitmap_image(&image_char_0_w3);
        spr_digit_tens_place_.x = 4;
        spr_digit_tens_place_.y = 1;

        spr_digit_ones_place_.set_bitmap_image(&image_char_0_w3);
        spr_digit_ones_place_.x = 8;
        spr_digit_ones_place_.y = 1;

    }

    void load_three_digit_image(
            LedMatrixBuffer &matrix_buffer,
            int16_t digit
    ) {

        if ( digit < 0 ) {
            digit = 0;
        } else if ( digit > 999 ) {
            digit = 999;
        } else {
        }

        spr_digit_hundreds_place_.set_bitmap_image(
                get_bitmap_w3_number(digit/100)
        );
        digit %= 100;

        spr_digit_tens_place_.set_bitmap_image(
                get_bitmap_w3_number(digit/10)
        );
        digit %= 10;

        spr_digit_ones_place_.set_bitmap_image(
                get_bitmap_w3_number(digit)
        );

        spr_digit_hundreds_place_.render_led_matrix_buffer(matrix_buffer);
        spr_digit_tens_place_.render_led_matrix_buffer(matrix_buffer);
        spr_digit_ones_place_.render_led_matrix_buffer(matrix_buffer);
    }

private:
    Sprite spr_digit_hundreds_place_;
    Sprite spr_digit_tens_place_;
    Sprite spr_digit_ones_place_;

    const BitmapImage * get_bitmap_w3_number(uint8_t number) {
        switch (number) {
        case 0: return &image_char_0_w3;
        case 1: return &image_char_1_w3;
        case 2: return &image_char_2_w3;
        case 3: return &image_char_3_w3;
        case 4: return &image_char_4_w3;
        case 5: return &image_char_5_w3;
        case 6: return &image_char_6_w3;
        case 7: return &image_char_7_w3;
        case 8: return &image_char_8_w3;
        case 9: return &image_char_9_w3;
        default: return nullptr;
        }
    }
};

#endif/*THREE_DIGIT_DISPLAY_HPP*/

