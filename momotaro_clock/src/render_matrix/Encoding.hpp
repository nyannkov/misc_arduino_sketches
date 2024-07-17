/*
 * MIT License
 * (https://opensource.org/license/mit/)
 *
 * Copyright (c) 2024 nyannkov
 */
#ifndef ENCODING_HPP
#define ENCODING_HPP

#include "BitmapImage.hpp"

extern bool load_image_of_font8x8(
    uint8_t *bmp_data,
    uint16_t bmp_width,
    uint16_t bmp_height,
    BitmapImage::DotType dot_type,
    const char *string_utf8
);


#endif/*ENCODING_HPP*/
