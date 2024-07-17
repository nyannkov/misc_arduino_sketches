/*
 * MIT License
 * (https://opensource.org/license/mit/)
 *
 * Copyright (c) 2024 nyannkov
 */
#ifndef LED_MATRIX_BUFFER_HPP
#define LED_MATRIX_BUFFER_HPP

constexpr int16_t LED_MATRIX_WIDTH = 12;
constexpr int16_t LED_MATRIX_HEIGHT = 8;

struct LedMatrixBuffer {
    uint8_t data[LED_MATRIX_HEIGHT][LED_MATRIX_WIDTH];
};

#endif/*LED_MATRIX_BUFFER_HPP*/
