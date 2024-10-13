/*
 * MIT License
 * (https://opensource.org/license/mit/)
 *
 * Copyright (c) 2024 nyannkov
 */
#ifndef SPRITE_HPP
#define SPRITE_HPP

#include "BitmapImage.hpp"
#include "LedMatrixBuffer.hpp"

struct Sprite {
    int16_t x;
    int16_t y;
    int16_t vx;
    int16_t vy;
    void initialize();
    void set_bitmap_image(const BitmapImage *bitmap_image);
    const BitmapImage *get_bitmap_image() const;
    bool render_led_matrix_buffer(LedMatrixBuffer &buffer);

private:
    const BitmapImage *image_;
};

#endif/*SPRITE_HPP*/
