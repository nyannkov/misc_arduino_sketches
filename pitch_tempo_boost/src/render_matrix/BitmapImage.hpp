/*
 * MIT License
 * (https://opensource.org/license/mit/)
 *
 * Copyright (c) 2024 nyannkov
 */
#ifndef BITMAP_IMAGE_HPP
#define BITMAP_IMAGE_HPP

#include <cstdint>

struct BitmapImage {
    const uint8_t *data;
    uint16_t width;
    uint16_t height;
    enum class DotType {
        Bit,
        Byte,
    } dot_type;
};


#endif/*BITMAP_IMAGE_HPP*/
