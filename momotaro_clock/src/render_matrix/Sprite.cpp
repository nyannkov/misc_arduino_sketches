/*
 * MIT License
 * (https://opensource.org/license/mit/)
 *
 * Copyright (c) 2024 nyannkov
 */
#include "Sprite.hpp"


void Sprite::initialize() {
    x = 0;
    y = 0;
    vx = 0;
    vy = 0;
    image_ = nullptr;
}

void Sprite::set_bitmap_image(const BitmapImage *bitmap_image) {
    image_ = bitmap_image;
}

const BitmapImage *Sprite::get_bitmap_image() const {
    return image_;
}

bool Sprite::render_led_matrix_buffer(LedMatrixBuffer &buffer) {

    int16_t r0, l0, t0, b0;
    int16_t r1, l1, t1, b1;
    uint8_t w, h;

    if ( !image_ ) {
        return false;
    }

    if ( ( image_->dot_type == BitmapImage::DotType::Bit ) &&
         ( (image_->width%8) != 0 )
    ) {
        return false;
    }

    l0 = 0;
    r0 = LED_MATRIX_WIDTH - 1;
    t0 = 0;
    b0 = LED_MATRIX_HEIGHT - 1;

    w = image_->width;
    h = image_->height;
    l1 = x;
    r1 = x + w - 1;
    t1 = y;
    b1 = y + h - 1;

    if ( (l0 <= r1) && (l1 <= r0) && (t0 <= b1) && (t1 <= b0) ) {
        int16_t row_s, col_s, row_e, col_e;

        col_s = (l1 < l0) ? l0 : l1;
        col_e = (r1 < r0) ? r1 : r0;
        row_s = (t1 < t0) ? t0 : t1;
        row_e = (b1 < b0) ? b1 : b0;

        for ( int16_t col = col_s; col <= col_e; col++ ) {
            for ( int16_t row = row_s; row <= row_e; row++ ) {
                if ( image_->dot_type == BitmapImage::DotType::Byte ) {
                    uint8_t pixel = image_->data[w*(row-t1) + col-l1];
                    if ( pixel != 0 ) {
                        buffer.data[row][col] = 1;
                    }
                } else if ( image_->dot_type == BitmapImage::DotType::Bit ) {
                    uint8_t pixel = image_->data[((col-l1)>>3) + (w>>3)*(row-t1)];
                    if ( ( pixel & (1<<(7-((col-l1)&0x7))) ) != 0 ) {
                        buffer.data[row][col] = 1;
                    }
                } else {
                    return false;
                }
            }
        }
        return true;
    } else {
        return false;
    }
}

