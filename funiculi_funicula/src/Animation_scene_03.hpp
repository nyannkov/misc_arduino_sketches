#ifndef ANIMATION_SCENE_03_HPP
#define ANIMATION_SCENE_03_HPP

#include "IAnimation.hpp"
#include "Resources.hpp"
#include "render_matrix/Sprite.hpp"


struct Animation_scene_03 : IAnimation {
    Animation_scene_03() {
    }
    ~Animation_scene_03() = default;

    void initialize() override {
        counter_ = 0;

        spr_ncoppa_.set_bitmap_image(&image_ncoppa_reverse_katakana);
        spr_ncoppa_.x = 3 - spr_ncoppa_.get_bitmap_image()->width;
        spr_ncoppa_.y = -6;
        spr_ncoppa_.vx = 1;
        spr_ncoppa_.vy = 0;

        spr_jammo_.set_bitmap_image(&image_jammo_reverse_katakana);
        spr_jammo_.x =  spr_ncoppa_.x - (spr_jammo_.get_bitmap_image()->width + 4);
        spr_jammo_.y = -39;
        spr_jammo_.vx = 1;
        spr_jammo_.vy = 0;

        spr_ja_.set_bitmap_image(&image_ja_katakana);
        spr_ja_.x =  spr_jammo_.x - (spr_ja_.get_bitmap_image()->width + 4);
        spr_ja_.y = -100;
        spr_ja_.vx = 2;
        spr_ja_.vy = 0;
    }

    bool update(int32_t param, LedMatrixBuffer &buffer) override {
        (void)param;

        if ( ++counter_ >= 2000 ) {
            counter_ = 0;

            spr_ncoppa_.x += 1;
            if ( spr_ncoppa_.y >= 0 ) {
                spr_ncoppa_.vy = (int16_t)(spr_ncoppa_.vy * -0.5F);
                spr_ncoppa_.y = 0;
            }
            spr_ncoppa_.y += spr_ncoppa_.vy;
            spr_ncoppa_.vy += 1;

            spr_jammo_.x += 1;
            if ( spr_jammo_.y >= 0 ) {
                spr_jammo_.vy = (int16_t)(spr_jammo_.vy * -0.5F);
                spr_jammo_.y = 0;
            }
            spr_jammo_.y += spr_jammo_.vy;
            spr_jammo_.vy += 1;

            spr_ja_.x += 1;
            if ( spr_ja_.y >= 0 ) {
                spr_ja_.vy = (int16_t)(spr_ja_.vy * -0.5F);
                spr_ja_.y = 0;
            }
            spr_ja_.y += spr_ja_.vy;
            spr_ja_.vy += 1;

            buffer = (LedMatrixBuffer){};
            spr_ncoppa_.render_led_matrix_buffer(buffer);
            spr_jammo_.render_led_matrix_buffer(buffer);
            spr_ja_.render_led_matrix_buffer(buffer);

            return true;
        } else {
            return false;
        }
    }

private:
    Sprite spr_ncoppa_;
    Sprite spr_jammo_;
    Sprite spr_ja_;
    uint32_t counter_;
};

#endif/*ANIMATION_SCENE_03_HPP*/
