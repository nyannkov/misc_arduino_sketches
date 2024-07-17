#ifndef DISPLAY_CLOCK_HPP
#define DISPLAY_CLOCK_HPP

#include "render_matrix/Sprite.hpp"
#include "Resources.hpp"

struct DisplayClock {

    DisplayClock() {}
    ~DisplayClock() = default;

    void initialize() {

        spr_hour_tens_.set_bitmap_image(nullptr);
        spr_hour_tens_.x = 0;
        spr_hour_tens_.y = 1;

        spr_hour_ones_.set_bitmap_image(&image_char_0_w3);
        spr_hour_ones_.x = 2;
        spr_hour_ones_.y = 1;

        spr_dot_.set_bitmap_image(&image_char_dot);
        spr_dot_.x = 5;
        spr_dot_.y = 7;

        spr_minute_tens_.set_bitmap_image(&image_char_0_w2);
        spr_minute_tens_.x = 6;
        spr_minute_tens_.y = 1;

        spr_minute_ones_.set_bitmap_image(&image_char_0_w3);
        spr_minute_ones_.x = 9;
        spr_minute_ones_.y = 1;
    }

    void load_display_clock_image(LedMatrixBuffer &matrix_buffer, uint8_t hour, uint8_t minute) {

        hour %= 12;
        if ( hour == 0 ) {
            hour = 12;
        }
        minute %= 60;

        // When the tens place of the hour is 0, it will not be displayed due to the 
        // width constraints of the display.
        spr_hour_tens_.set_bitmap_image( (hour >= 10) ? &image_char_1_w2 : nullptr );
        spr_hour_ones_.set_bitmap_image( get_bitmap_w3_number(hour%10) );
        spr_minute_tens_.set_bitmap_image( get_bitmap_w2_number(minute/10) );
        spr_minute_ones_.set_bitmap_image( get_bitmap_w3_number(minute%10) );

        spr_hour_tens_.render_led_matrix_buffer(matrix_buffer);
        spr_hour_ones_.render_led_matrix_buffer(matrix_buffer);
        spr_minute_tens_.render_led_matrix_buffer(matrix_buffer);
        spr_minute_ones_.render_led_matrix_buffer(matrix_buffer);

        spr_dot_.render_led_matrix_buffer(matrix_buffer);
    }

private:
    Sprite spr_hour_tens_;
    Sprite spr_hour_ones_;
    Sprite spr_dot_;
    Sprite spr_minute_tens_;
    Sprite spr_minute_ones_;
    const BitmapImage * get_bitmap_w2_number(uint8_t number) {
        switch (number) {
        case 0: return &image_char_0_w2;
        case 1: return &image_char_1_w2;
        case 2: return &image_char_2_w2;
        case 3: return &image_char_3_w2;
        case 4: return &image_char_4_w2;
        case 5: return &image_char_5_w2;
        default: return nullptr;
        }
    }

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

#endif/*DISPLAY_CLOCK_HPP*/

