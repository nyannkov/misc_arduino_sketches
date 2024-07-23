#ifndef DISPLAY_MOMOTARO_HPP
#define DISPLAY_MOMOTARO_HPP

#include <cstring>
#include "render_matrix/Sprite.hpp"
#include "render_matrix/Encoding.hpp"
#include "Resources.hpp"

constexpr size_t MAX_LYRICS_LENGTH = 80;
constexpr size_t FONT_HEIGHT = 8;

struct DisplayMomotaro {

    DisplayMomotaro() {}
    ~DisplayMomotaro() = default;

    void initialize() {

        spr_peach_.set_bitmap_image(&image_peach);
        spr_peach_.x = 12;
        spr_peach_.y = 1;

        std::memset(bmp_lyrics_1_, 0, sizeof(bmp_lyrics_1_));
        load_image_of_font8x8(bmp_lyrics_1_, MAX_LYRICS_LENGTH, FONT_HEIGHT, BitmapImage::DotType::Bit, "ﾓｰﾓﾀﾛｳｻﾝ ﾓﾓﾀﾛｻﾝ");
        image_lyrics_1_.data = bmp_lyrics_1_;
        image_lyrics_1_.width = MAX_LYRICS_LENGTH;
        image_lyrics_1_.height = FONT_HEIGHT;
        image_lyrics_1_.dot_type = BitmapImage::DotType::Bit;
        spr_lyrics_1_.set_bitmap_image(&image_lyrics_1_);
        spr_lyrics_1_.x = -100;
        spr_lyrics_1_.y = 0;

        std::memset(bmp_lyrics_2_, 0, sizeof(bmp_lyrics_2_));
        load_image_of_font8x8(bmp_lyrics_2_, MAX_LYRICS_LENGTH, FONT_HEIGHT, BitmapImage::DotType::Bit, "ｵｺｼﾆﾂｹﾀ ｷﾋﾞﾀﾞﾝｺﾞ");
        image_lyrics_2_.data = bmp_lyrics_2_;
        image_lyrics_2_.width = MAX_LYRICS_LENGTH;
        image_lyrics_2_.height = FONT_HEIGHT;
        image_lyrics_2_.dot_type = BitmapImage::DotType::Bit;
        spr_lyrics_2_.set_bitmap_image(&image_lyrics_2_);
        spr_lyrics_2_.x = -100;
        spr_lyrics_2_.y = 0;

        std::memset(bmp_lyrics_3_, 0, sizeof(bmp_lyrics_3_));
        load_image_of_font8x8(bmp_lyrics_3_, MAX_LYRICS_LENGTH, FONT_HEIGHT, BitmapImage::DotType::Bit, "ﾋﾄﾂ ﾜﾀｼﾆ ｸﾀﾞｻｲﾅ");
        image_lyrics_3_.data = bmp_lyrics_3_;
        image_lyrics_3_.width = MAX_LYRICS_LENGTH;
        image_lyrics_3_.height = FONT_HEIGHT;
        image_lyrics_3_.dot_type = BitmapImage::DotType::Bit;
        spr_lyrics_3_.set_bitmap_image(&image_lyrics_3_);
        spr_lyrics_3_.x = -100;
        spr_lyrics_3_.y = 0;

        std::memset(bmp_lyrics_4_, 0, sizeof(bmp_lyrics_4_));
        load_image_of_font8x8(bmp_lyrics_4_, MAX_LYRICS_LENGTH, FONT_HEIGHT, BitmapImage::DotType::Bit, "ｷﾋﾞﾀﾞﾝｺﾞ ｷﾋﾞﾀﾞﾝｺﾞ");

        scene_id_ = 0;
        counter_ = 0;

        period_peach_ = 2000;
        period_lyrics_ = 700;

        kibidango_mode_ = false;

        momotaro_loop_counter_ = 0;
    }

    void set_animation(int32_t scene_id) {
        scene_id_ = scene_id;
        switch ( scene_id_ ) {
        case 1:
            spr_peach_.x = 12;
            spr_peach_.y = 1;
            break;

        case 2:
            momotaro_loop_counter_++;
            spr_lyrics_1_.x = 12;
            spr_lyrics_1_.y = 0;
            if ( kibidango_mode_ && ( momotaro_loop_counter_ == 2 ) ) {
                image_lyrics_2_.data = bmp_lyrics_4_;
            }
            break;

        case 3:
            spr_lyrics_2_.x = 12;
            spr_lyrics_2_.y = 0;
            if ( kibidango_mode_ && ( momotaro_loop_counter_ == 2 ) ) {
                image_lyrics_3_.data = bmp_lyrics_4_;
            }
            break;

        case 4:
            spr_lyrics_3_.x = 12;
            spr_lyrics_3_.y = 0;
            if ( kibidango_mode_ && ( momotaro_loop_counter_ == 1 ) ) {
                image_lyrics_1_.data = bmp_lyrics_4_;
            }
            break;

        default:
            break;
        }
    }

    int32_t get_scene_id() const {
        return scene_id_;
    }

    void set_speed(int32_t speed) {
        switch ( speed ) {
        case 1:
            period_peach_ = 1000;
            period_lyrics_ = 400;
            break;

        case 2:
            period_peach_ = 900;
            period_lyrics_ = 350;
            break;

        case 3:
            period_peach_ = 800;
            period_lyrics_ = 310;
            break;

        case 4:
            period_peach_ = 700;
            period_lyrics_ = 290;
            break;

        case 5:
            period_peach_ = 670;
            period_lyrics_ = 270;
            break;

        case 6:
            period_peach_ = 650;
            period_lyrics_ = 250;
            break;

        case 7:
            period_peach_ = 630;
            period_lyrics_ = 220;
            break;

        case 8:
            period_peach_ = 610;
            period_lyrics_ = 190;
            break;

        case 9:
            period_peach_ = 590;
            period_lyrics_ = 180;
            break;

        case 10:
            period_peach_ = 570;
            period_lyrics_ = 170;
            break;

        default:
            period_peach_ = 550;
            period_lyrics_ = 160;
            break;
        }
    }

    void enter_kibidango_mode() {

        image_lyrics_1_.data = bmp_lyrics_1_;
        image_lyrics_1_.width = MAX_LYRICS_LENGTH;
        image_lyrics_1_.height = FONT_HEIGHT;
        image_lyrics_1_.dot_type = BitmapImage::DotType::Bit;
        spr_lyrics_1_.set_bitmap_image(&image_lyrics_1_);
        spr_lyrics_1_.x = -100;
        spr_lyrics_1_.y = 0;

        image_lyrics_2_.data = bmp_lyrics_2_;
        image_lyrics_2_.width = MAX_LYRICS_LENGTH;
        image_lyrics_2_.height = FONT_HEIGHT;
        image_lyrics_2_.dot_type = BitmapImage::DotType::Bit;
        spr_lyrics_2_.set_bitmap_image(&image_lyrics_2_);
        spr_lyrics_2_.x = -100;
        spr_lyrics_2_.y = 0;

        std::memset(bmp_lyrics_3_, 0, sizeof(bmp_lyrics_3_));
        load_image_of_font8x8(bmp_lyrics_3_, MAX_LYRICS_LENGTH, FONT_HEIGHT, BitmapImage::DotType::Bit, "ﾋﾄﾂ ﾜﾀｼﾆ ｷﾋﾞﾀﾞﾝｺﾞ");
        image_lyrics_3_.data = bmp_lyrics_3_;
        image_lyrics_3_.width = MAX_LYRICS_LENGTH;
        image_lyrics_3_.height = FONT_HEIGHT;
        image_lyrics_3_.dot_type = BitmapImage::DotType::Bit;
        spr_lyrics_3_.set_bitmap_image(&image_lyrics_3_);
        spr_lyrics_3_.x = -100;
        spr_lyrics_3_.y = 0;

        kibidango_mode_ = true;
    }

    bool update_animation(LedMatrixBuffer &matrix_buffer) {

        if ( scene_id_ == 1 ) {
            if ( counter_++ > period_peach_ ) {
                counter_ = 0;
                spr_peach_.x--;
                spr_peach_.y = (spr_peach_.y == 1) ? 2 : 1;
                matrix_buffer = (LedMatrixBuffer){};
                spr_peach_.render_led_matrix_buffer(matrix_buffer);
            }
        } else {
            if ( counter_++ > period_lyrics_ ) {
                counter_ = 0;

                spr_lyrics_1_.x--;
                spr_lyrics_2_.x--;
                spr_lyrics_3_.x--;

                if ( kibidango_mode_ ) {
                    spr_lyrics_1_.y ^= 1;
                    spr_lyrics_2_.y ^= 1;
                    spr_lyrics_3_.y ^= 1;
                }

                matrix_buffer = (LedMatrixBuffer){};
                spr_lyrics_1_.render_led_matrix_buffer(matrix_buffer);
                spr_lyrics_2_.render_led_matrix_buffer(matrix_buffer);
                spr_lyrics_3_.render_led_matrix_buffer(matrix_buffer);
            }
        }

        return true;
    }

private:
    Sprite spr_peach_;
    Sprite spr_lyrics_1_;
    Sprite spr_lyrics_2_;
    Sprite spr_lyrics_3_;
    int32_t scene_id_;
    uint32_t counter_;
    uint32_t period_peach_;
    uint32_t period_lyrics_;
    uint8_t bmp_lyrics_1_[FONT_HEIGHT*MAX_LYRICS_LENGTH/8];
    uint8_t bmp_lyrics_2_[FONT_HEIGHT*MAX_LYRICS_LENGTH/8];
    uint8_t bmp_lyrics_3_[FONT_HEIGHT*MAX_LYRICS_LENGTH/8];
    uint8_t bmp_lyrics_4_[FONT_HEIGHT*MAX_LYRICS_LENGTH/8];
    BitmapImage image_lyrics_1_;
    BitmapImage image_lyrics_2_;
    BitmapImage image_lyrics_3_;
    bool kibidango_mode_;
    uint32_t momotaro_loop_counter_;
};

#endif/*CLOCK_DISPLAY_HPP*/

